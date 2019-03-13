//    |  /           |
//    ' /   __| _` | __|  _ \   __|
//    . \  |   (   | |   (   |\__ `
//   _|\_\_|  \__,_|\__|\___/ ____/
//                   Multi-Physics
//
//  License:         BSD License
//                   Kratos default license: kratos/license.txt
//
//  Main authors:    Miguel Maso Sotomayor
//


// System includes


// External includes


// Project includes
#include "includes/checks.h"
#include "initial_perturbation_process.h"


namespace Kratos
{

InitialPerturbationProcess::InitialPerturbationProcess(
    ModelPart& rThisModelPart,
    NodePointerType pNode,
    Parameters& rThisParameters
) : Process(), mrModelPart(rThisModelPart)
{
    ValidateParameters(rThisParameters);
    mSourcePoints.push_back(pNode);
    Check();
}


InitialPerturbationProcess::InitialPerturbationProcess(
    ModelPart& rThisModelPart,
    NodesArrayType& rSourcePoints,
    Parameters& rThisParameters
) : Process(), mrModelPart(rThisModelPart)
{
    ValidateParameters(rThisParameters);
    mSourcePoints = rSourcePoints;
    Check();
}


int InitialPerturbationProcess::Check()
{
    KRATOS_TRY
    if (mrModelPart.Nodes().size() != 0) {
        const auto& r_node = *mrModelPart.NodesBegin();
        KRATOS_CHECK_VARIABLE_IN_NODAL_DATA(mVariable, r_node);
    }
    KRATOS_CHECK(mInfluenceDistance <= std::numeric_limits<double>::epsilon());
    return 0;
    KRATOS_CATCH("")
}


void InitialPerturbationProcess::Execute()
{
    ExecuteBeforeSolutionLoop();
}


void InitialPerturbationProcess::ExecuteBeforeSolutionLoop()
{
    for (int i = 0; i < static_cast<int>(mrModelPart.Nodes().size()); i++)
    {
        auto i_node = mrModelPart.NodesBegin() + i;
        double distance = ComputeDistance(*i_node.base());
        double& r_value = i_node->FastGetSolutionStepValue(mVariable);
        r_value = ComputeInitialValue(distance);
    }    
}


double InitialPerturbationProcess::ComputeDistance(NodePointerType pNode)
{
    array_1d<double, 3>& coord = pNode->Coordinates();
    double sqr_distance = 0;
    for (IndexType i = 0; i < mSourcePoints.size(); i++)
    {
        auto search_node = mSourcePoints.begin() + i;
        array_1d<double, 3>& source_coord = search_node->Coordinates();
        sqr_distance = std::max(sqr_distance, PointPointSquareDistance(coord, source_coord));
    }
    return std::sqrt(sqr_distance);
}


double InitialPerturbationProcess::PointPointSquareDistance(array_1d<double, 3>& rCoordA, array_1d<double, 3>& rCoordB)
{
    return std::pow(rCoordA[0] - rCoordB[0], 2) + std::pow(rCoordA[1] - rCoordB[1], 2) + std::pow(rCoordA[2] - rCoordB[2], 2);
}


double InitialPerturbationProcess::ComputeInitialValue(double& rDistance)
{
    double result = mDefaultValue;
    if (rDistance < mInfluenceDistance)
        result += 0.5 * mPerturbation * (1 + std::cos(mHalfWaveNumber * rDistance));
    return result;
}


void InitialPerturbationProcess::ValidateParameters(Parameters& rParameters)
{
    // default parameters
    Parameters default_parameters = Parameters(R"(
    {
        "variable_name"              : "FREE_SURFACE_ELEVATION",
        "default_value"              : 0.0,
        "distance_of_influence"      : 1.0,
        "maximum_perturbation_value" : 1.0
    })");
    rParameters.ValidateAndAssignDefaults(default_parameters);

    // Initialization of member variables
    mVariable = KratosComponents< Variable<double> >::Get(rParameters["error_variable"].GetString());
    mDefaultValue = rParameters["default_value"].GetDouble();
    mInfluenceDistance = rParameters["distance_of_influence"].GetDouble();
    mPerturbation = rParameters["maximum_perturbation_value"].GetDouble();
    mHalfWaveNumber = std::acos(-1) / mInfluenceDistance;
}

}  // namespace Kratos.


