//    |  /           |
//    ' /   __| _` | __|  _ \   __|
//    . \  |   (   | |   (   |\__ `
//   _|\_\_|  \__,_|\__|\___/ ____/
//                   Multi-Physics 
//
//  License:		 BSD License 
//					 Kratos default license: kratos/license.txt
//
//  Main authors:    Miguel Maso Sotomayor
//


// System includes


// External includes 


// Project includes
#include "includes/define.h"
#include "includes/variables.h"
#include "uniform_refine_utility.h"
#include "utilities/sub_model_parts_list_utility.h"


namespace Kratos
{
/// Default constructor
template< unsigned int TDim>
UniformRefineUtility<TDim>::UniformRefineUtility(ModelPart& rModelPart, int RefinementLevel) :
    mrModelPart(rModelPart),
    mFinalRefinementLevel(RefinementLevel)
{
    // Initialize the member variables storing the Id
    mLastNodeId = 0;
    mLastElemId = 0;
    mLastCondId = 0;

    // Get the last node id
    const int nnodes = mrModelPart.Nodes().size();
    for (int i = 0; i < nnodes; i++)
    {
        ModelPart::NodesContainerType::iterator inode = mrModelPart.NodesBegin() + i;
        if (inode->Id() > mLastNodeId)
            mLastNodeId = inode->Id();
    }

    // Get the elements id
    const int n_elements = mrModelPart.Elements().size();
    for (int i = 0; i < n_elements; i++)
    {
        ModelPart::ElementsContainerType::iterator ielement = mrModelPart.ElementsBegin() + i;
        if (ielement->Id() > mLastElemId)
            mLastElemId = ielement->Id();
    }

    // Get the conditions id
    const int n_conditions = mrModelPart.Conditions().size();
    for (int i = 0; i < n_conditions; i++)
    {
        ModelPart::ConditionsContainerType::iterator icondition = mrModelPart.ConditionsBegin() + i;
        if (icondition->Id() > mLastCondId)
            mLastCondId = icondition->Id();
    }

    mStepDataSize = mrModelPart.GetNodalSolutionStepDataSize();
    mBufferSize = mrModelPart.GetBufferSize();
    mDofs = mrModelPart.NodesBegin()->GetDofs();

    // Compute the sub model part maps
    SubModelPartsListUtility sub_model_parts_list(mrModelPart);
    sub_model_parts_list.ComputeSubModelPartsList(mNodesColorMap, mCondColorMap, mElemColorMap, mColors);
}


/// Destructor
template< unsigned int TDim>
UniformRefineUtility<TDim>::~UniformRefineUtility() {}


/// Turn back information as a string.
template< unsigned int TDim>
std::string UniformRefineUtility<TDim>::Info() const {
    return "Uniform refine utility.";
}


/// Print information about this object.
template< unsigned int TDim>
void UniformRefineUtility<TDim>::PrintInfo(std::ostream& rOStream) const {
    rOStream << "Uniform refine utility.";
}


/// Print object's data.
template< unsigned int TDim>
void UniformRefineUtility<TDim>::PrintData(std::ostream& rOStream) const {
    rOStream << "Uniform refine utility constructed with:\n";
    rOStream << "   Model part: " << mrModelPart.Info() << "\n";
    rOStream << "   Final refinement level: " << mFinalRefinementLevel << "\n";
}


/// Execute the refinement until the final refinement level is reached
template< unsigned int TDim>
void UniformRefineUtility<TDim>::Refine()
{
    std::cout << "--------------------------------------" << std::endl;
    std::cout << "----------- Before remeshing ---------" << std::endl;
    std::cout << "--------------------------------------" << std::endl;
    KRATOS_WATCH(mrModelPart)
    
    // Get the lowest refinement level
    int minimum_refinement_level = 1e6;
    const int n_elements = mrModelPart.Elements().size();
    for (int i = 0; i < n_elements; i++)
    {
        ModelPart::ElementsContainerType::iterator ielement = mrModelPart.ElementsBegin() + i;
        if (ielement->GetValue(REFINEMENT_LEVEL) < minimum_refinement_level)
            minimum_refinement_level = ielement->GetValue(REFINEMENT_LEVEL);
    }

    for (int level = minimum_refinement_level; level < mFinalRefinementLevel; level++)
    {
        RefineLevel(level);
    }

    std::cout << "--------------------------------------" << std::endl;
    std::cout << "----------- After remeshing ----------" << std::endl;
    std::cout << "--------------------------------------" << std::endl;
    KRATOS_WATCH(mrModelPart)
}


/// Execute the refinement once
template< unsigned int TDim>
void UniformRefineUtility<TDim>::RefineLevel(const int& rThisLevel)
{
    // Initialize the entities Id lists
    std::vector<int> elements_id;
    std::vector<int> conditions_id;


    // Get the elements id
    const int n_elements = mrModelPart.Elements().size();
    for (int i = 0; i < n_elements; i++)
    {
        ModelPart::ElementsContainerType::iterator ielement = mrModelPart.ElementsBegin() + i;

        // Check the refinement level of the origin elements
        int step_refine_level = ielement->GetValue(REFINEMENT_LEVEL);
        if (step_refine_level == rThisLevel)
            elements_id.push_back(ielement->Id());
    }

    // Get the conditions id
    const int n_conditions = mrModelPart.Conditions().size();
    for (int i = 0; i < n_conditions; i++)
    {
        ModelPart::ConditionsContainerType::iterator icondition = mrModelPart.ConditionsBegin() + i;

        // Check the refinement level of the origin conditions
        int step_refine_level = icondition->GetValue(REFINEMENT_LEVEL);
        if (step_refine_level == rThisLevel)
            conditions_id.push_back(icondition->Id());
    }

    // Loop the origin elements to create the middle nodes
    for (int id : elements_id)
    {
        // Get the element
        Element::Pointer p_element = mrModelPart.Elements()(id);

        // Get the refinement level of the origin element
        int step_refine_level = rThisLevel + 1;

        // Get the geometry
        Geometry<NodeType>& geom = p_element->GetGeometry();

        // Loop the edges of the father element and get the nodes
        for (auto edge : geom.Edges())
            CreateNodeInEdge(edge, step_refine_level);
        
        if (geom.GetGeometryType() == GeometryData::KratosGeometryType::Kratos_Quadrilateral2D4)
            CreateNodeInFace( geom, step_refine_level );

        // Encontrar el lugar para ejecutar SubModelPartsColors
    }

    // TODO: add OMP
    // Create the elements
    for (int id : elements_id)
    {
        // Get the element
        Element::Pointer p_element = mrModelPart.Elements()(id);

        // Get the refinement level of the origin element
        int step_refine_level = rThisLevel + 1;

        // Get the geometry
        Geometry<NodeType>& geom = p_element->GetGeometry();

        if (geom.GetGeometryType() == GeometryData::KratosGeometryType::Kratos_Triangle2D3)
        {
            // Loop the edges of the father element and get the nodes
            int i_edge = 0;
            std::array<NodeType::Pointer, 3> middle_nodes;
            for (auto edge : geom.Edges())
                middle_nodes[i_edge++] = GetNodeInEdge(edge);

            // Create the sub elements
            std::vector<NodeType::Pointer> sub_element_nodes(3);
            for (int position = 0; position < 4; position++)
            {
                sub_element_nodes = GetSubTriangleNodes(position, geom, middle_nodes);
                CreateElement(p_element, sub_element_nodes, step_refine_level);
            }
        }
        else if (geom.GetGeometryType() == GeometryData::KratosGeometryType::Kratos_Quadrilateral2D4)
        {
            // Loop the edges of the father element and get the nodes
            int i_edge = 0;
            std::array<NodeType::Pointer, 5> middle_nodes;
            for (auto edge : geom.Edges())
                middle_nodes[i_edge++] = GetNodeInEdge(edge);
            middle_nodes[4] = GetNodeInFace( geom );

            // Create the sub elements
            std::vector<NodeType::Pointer> sub_element_nodes(4);
            for (int position = 0; position < 4; position++)
            {
                sub_element_nodes = GetSubQuadrilateralNodes(position, geom, middle_nodes);
                CreateElement(p_element, sub_element_nodes, step_refine_level);
            }
        }
        else
        {
            KRATOS_ERROR << "Your geometry contains " << geom.GetGeometryType() <<" which cannot be remeshed" << std::endl;
        }

        // Once we have created all the sub elements, the origin element must be deleted
        p_element->Set(TO_ERASE, true);
    }

    mrModelPart.RemoveElementsFromAllLevels(TO_ERASE);

    // Loop the origin conditions
    for (const int id : conditions_id)
    {
        // Get the condition
        Condition::Pointer p_condition = mrModelPart.Conditions()(id);

        // Check the refinement level of the origin condition
        // int step_refine_level = rThisLevel + 1;
        // THIRD: Create the conditions

        /* Do some stuff here */

    }

    mrModelPart.RemoveConditionsFromAllLevels(TO_ERASE);

}


/// Create a middle node on an edge. If the node does not exist, it creates one
template <unsigned int TDim>
void UniformRefineUtility<TDim>::CreateNodeInEdge(
    const EdgeType& rEdge,
    const int& rRefinementLevel
    )
{
    // Get the middle node key
    std::pair<int, int> node_key;
    node_key = std::minmax(rEdge(0)->Id(), rEdge(1)->Id());

    // Check if the node is not yet created
    auto search = mNodesMap.find(node_key);
    if (search == mNodesMap.end() )
    {
        // Create the new node
        double new_x = 0.5*rEdge(0)->X() + 0.5*rEdge(1)->X();
        double new_y = 0.5*rEdge(0)->Y() + 0.5*rEdge(1)->Y();
        double new_z = 0.5*rEdge(0)->Z() + 0.5*rEdge(1)->Z();
        NodeType::Pointer middle_node = mrModelPart.CreateNewNode(++mLastNodeId, new_x, new_y, new_z);

        // Store the node key in the map
        mNodesMap.insert( std::pair< std::pair<int, int>, int > (node_key, middle_node->Id()) );

        // interpolate the variables
        CalculateNodalStepData(middle_node, rEdge(0), rEdge(1));

        // Set the refinement level
        int& this_node_level = middle_node->GetValue(REFINEMENT_LEVEL);
        this_node_level = rRefinementLevel;

        // Set the DoF's
        for (typename NodeType::DofsContainerType::const_iterator it_dof = mDofs.begin(); it_dof != mDofs.end(); ++it_dof)
            middle_node->pAddDof(*it_dof);
        
        // Add the element to the sub model parts
        int key0 = mNodesColorMap[rEdge(0)->Id()];
        int key1 = mNodesColorMap[rEdge(1)->Id()];
        // int key = SubModelPartsListUtility::IntersectKeys(key0, key1, mColors);
        // if (key != 0)  // NOTE: key==0 is the main model part
        // {
        //     for (std::string sub_name : mColors[key])
        //     {
        //         ModelPart& sub_model_part = SubModelPartsListUtility::GetRecursiveSubModelPart(mrModelPart, sub_name);
        //         sub_model_part.AddElement(sub_element);
        //     }
        // }
        // mNodesColorMap.insert( std::pair<int,int>(sub_element->Id(), key) );
    }
}


/// Get the middle node on an edge
template <unsigned int TDim>
Node<3>::Pointer UniformRefineUtility<TDim>::GetNodeInEdge(const EdgeType& rEdge)
{
    // Initialize the output
    NodeType::Pointer middle_node;

    // Get the middle node key
    std::pair<int, int> node_key;
    node_key = std::minmax(rEdge(0)->Id(), rEdge(1)->Id());

    // Check if the node exist
    auto search = mNodesMap.find(node_key);
    if (search != mNodesMap.end() )
    {
        middle_node = mrModelPart.Nodes()(search->second);
    }
    else
    {
        KRATOS_WARNING("UniformRefineProcess") << "Middle node not found in edge" << rEdge << std::endl;
    }

    return middle_node;
}


/// Get the middle node on a face defined by four nodes. If the node does not exist, it creates one
template< unsigned int TDim>
void UniformRefineUtility<TDim>::CreateNodeInFace(
    const FaceType& rFace,
    const int& rRefinementLevel
    )
{
    // Get the middle node key
    std::array<int, 4> node_key = {rFace(0)->Id(), rFace(1)->Id(), rFace(2)->Id(), rFace(3)->Id()};
    std::sort(node_key.begin(), node_key.end());
    // node_key = std::sort(rFace(0)->Id(), rFace(1)->Id(), rFace(2)->Id(), rFace(3)->Id());

    // Check if the node is not yet created
    auto search = mNodesInFaceMap.find(node_key);
    if (search == mNodesInFaceMap.end() )
    {
        // Create the new node
        double new_x = 0.25*rFace(0)->X() + 0.25*rFace(1)->X() + 0.25*rFace(2)->X() + 0.25*rFace(3)->X();
        double new_y = 0.25*rFace(0)->Y() + 0.25*rFace(1)->Y() + 0.25*rFace(2)->Y() + 0.25*rFace(3)->Y();
        double new_z = 0.25*rFace(0)->Z() + 0.25*rFace(1)->Z() + 0.25*rFace(2)->Z() + 0.25*rFace(3)->Z();
        Node<3>::Pointer middle_node = mrModelPart.CreateNewNode(++mLastNodeId, new_x, new_y, new_z);

        // Store the node key in the map
        mNodesInFaceMap.insert( std::pair< std::array<int, 4>, int > (node_key, middle_node->Id()) );

        // interpolate the variables
        CalculateNodalStepData(middle_node, rFace(0), rFace(1));

        // Set the refinement level
        int& this_node_level = middle_node->GetValue(REFINEMENT_LEVEL);
        this_node_level = rRefinementLevel;

        // Set the DoF's
        for (typename NodeType::DofsContainerType::const_iterator it_dof = mDofs.begin(); it_dof != mDofs.end(); ++it_dof)
            middle_node->pAddDof(*it_dof);
    }
}


/// Get the middle node on a face defined by four nodes. If the node does not exist, it creates one
template< unsigned int TDim>
Node<3>::Pointer UniformRefineUtility<TDim>::GetNodeInFace(const FaceType& rFace)
{
    // Initialize the output
    NodeType::Pointer middle_node;

    // Get the middle node key
    std::array<int, 4> node_key = {rFace(0)->Id(), rFace(1)->Id(), rFace(2)->Id(), rFace(3)->Id()};
    std::sort(node_key.begin(), node_key.end());

    // Check if the node exist
    auto search = mNodesInFaceMap.find(node_key);
    if (search != mNodesInFaceMap.end() )
    {
        middle_node = mrModelPart.Nodes()(search->second);
    }
    else
    {
        KRATOS_WARNING("UniformRefineProcess") << "Middle node not found in edge" << rFace << std::endl;
    }

    return middle_node;
}


/// Compute the nodal data of a node
template< unsigned int TDim >
void UniformRefineUtility<TDim>::CalculateNodalStepData(
    NodeType::Pointer pNewNode, 
    const NodeType::Pointer pNode0, 
    const NodeType::Pointer pNode1
    )
{
    for (unsigned int step = 0; step < mBufferSize; step++)
    {
        double* new_node_data = pNewNode->SolutionStepData().Data(step);

        const double* node_data_0 = pNode0->SolutionStepData().Data(step);
        const double* node_data_1 = pNode1->SolutionStepData().Data(step);

        for (unsigned int variable = 0; variable < mStepDataSize; variable++)
            new_node_data[variable] = 0.5 * node_data_0[variable] + 0.5 * node_data_1[variable];
    }
}


/// Create a sub element
template<unsigned int TDim>
void UniformRefineUtility<TDim>::CreateElement(
    Element::Pointer pOriginElement,
    std::vector<NodeType::Pointer> ThisNodes,
    const int& rRefinementLevel
    )
{
    Element::Pointer sub_element = pOriginElement->Create(++mLastElemId, ThisNodes, pOriginElement->pGetProperties());
    
    if (sub_element != nullptr) 
    {
        // Add the element to the origin model part
        mrModelPart.AddElement(sub_element);

        // Set the refinement level
        int& this_elem_level = sub_element->GetValue(REFINEMENT_LEVEL);
        this_elem_level = rRefinementLevel;

        // Add the element to the sub model parts
        int key = mElemColorMap[pOriginElement->Id()];
        if (key != 0)  // NOTE: key==0 is the main model part
        {
            for (std::string sub_name : mColors[key])
            {
                ModelPart& sub_model_part = SubModelPartsListUtility::GetRecursiveSubModelPart(mrModelPart, sub_name);
                sub_model_part.AddElement(sub_element);
            }
        }
        mElemColorMap.insert( std::pair<int,int>(sub_element->Id(), key) );
    }

}


/// Return the nodes defining the i-subtriangle
template<unsigned int TDim>
std::vector<Node<3>::Pointer> UniformRefineUtility<TDim>::GetSubTriangleNodes(
    int Position,
    Geometry<NodeType>& rGeom,
    std::array<NodeType::Pointer, 3>& rMiddleNodes
    )
{
    std::vector<NodeType::Pointer> sub_element_nodes(3);

    if (Position == 0)
    {
        // First sub element
        sub_element_nodes[0] = rGeom.pGetPoint(0);
        sub_element_nodes[1] = rMiddleNodes[0];
        sub_element_nodes[2] = rMiddleNodes[2];
    }
    else if (Position == 1)
    {
        // Second sub element
        sub_element_nodes[0] = rGeom.pGetPoint(1);
        sub_element_nodes[1] = rMiddleNodes[1];
        sub_element_nodes[2] = rMiddleNodes[0];
    }
    else if (Position == 2)
    {
        // Third sub element
        sub_element_nodes[0] = rGeom.pGetPoint(2);
        sub_element_nodes[1] = rMiddleNodes[2];
        sub_element_nodes[2] = rMiddleNodes[1];
    }
    else if (Position == 3)
    {
        // Fourth sub element (inner element)
        sub_element_nodes[0] = rMiddleNodes[0];
        sub_element_nodes[1] = rMiddleNodes[1];
        sub_element_nodes[2] = rMiddleNodes[2];
    }
    else
    {
        KRATOS_ERROR << "Attempting to get " << Position << " sub element inside a triangle" << std::endl;
    }

    return sub_element_nodes;
}

/// Return the nodes defining the i-subquadrilateral
template<unsigned int TDim>
std::vector<Node<3>::Pointer> UniformRefineUtility<TDim>::GetSubQuadrilateralNodes(
    int Position,
    Geometry<NodeType>& rGeom,
    std::array<NodeType::Pointer, 5>& rMiddleNodes
    )
{
    std::vector<NodeType::Pointer> sub_element_nodes(4);

    if (Position == 0)
    {
        // First sub element
        sub_element_nodes[0] = rGeom.pGetPoint(0);
        sub_element_nodes[1] = rMiddleNodes[0];
        sub_element_nodes[2] = rMiddleNodes[4];
        sub_element_nodes[3] = rMiddleNodes[3];
    }
    else if (Position == 1)
    {
        // Second sub element
        sub_element_nodes[0] = rGeom.pGetPoint(1);
        sub_element_nodes[1] = rMiddleNodes[1];
        sub_element_nodes[2] = rMiddleNodes[4];
        sub_element_nodes[3] = rMiddleNodes[0];
    }
    else if (Position == 2)
    {
        // Third sub element
        sub_element_nodes[0] = rGeom.pGetPoint(2);
        sub_element_nodes[1] = rMiddleNodes[2];
        sub_element_nodes[2] = rMiddleNodes[4];
        sub_element_nodes[3] = rMiddleNodes[1];
    }
    else if (Position == 3)
    {
        // Fourth sub element
        sub_element_nodes[0] = rGeom.pGetPoint(3);
        sub_element_nodes[1] = rMiddleNodes[3];
        sub_element_nodes[2] = rMiddleNodes[4];
        sub_element_nodes[3] = rMiddleNodes[2];
    }
    else
    {
        KRATOS_ERROR << "Attempting to get " << Position << " sub element inside a quadrilateral" << std::endl;
    }

    return sub_element_nodes;
}



template class UniformRefineUtility<2>;

}  // namespace Kratos.


