//    |  /           |
//    ' /   __| _` | __|  _ \   __|
//    . \  |   (   | |   (   |\__ `
//   _|\_\_|  \__,_|\__|\___/ ____/
//                   Multi-Physics
//
//  License:         BSD License
//                   Kratos default license: kratos/license.txt
//
//  Main author:     Riccardo Tosi
//

#ifndef KRATOS_SYMBOLIC_QS_CONVECTION_DIFFUSION_EXPLICIT_H
#define KRATOS_SYMBOLIC_QS_CONVECTION_DIFFUSION_EXPLICIT_H

// System includes


// External includes


// Project includes
#include "includes/element.h"
#include "includes/serializer.h"
#include "includes/checks.h"
#include "includes/variables.h"
#include "includes/convection_diffusion_settings.h"
#include "geometries/geometry.h"
#include "utilities/geometry_utilities.h"
#include "includes/cfd_variables.h"
#include "convection_diffusion_application_variables.h"

namespace Kratos
{

///@name Kratos Globals
///@{

///@}
///@name Type Definitions
///@{

///@}
///@name  Enum's
///@{

///@}
///@name  Functions
///@{

///@}
///@name Kratos Classes
///@{

/**
 * @class SymbolicQSConvectionDiffusionExplicit
 * @ingroup ConvectionDiffusionApplication
 * @brief This element solves the convection-diffusion equation, stabilized with
 * algebraic subgrid scale or orthogonal subgrid scale.
 * @details This element solves the convection-diffusion equation:
 * $ \frac{\partial \phi}{\partial t} + v \cdot  \nabla \phi + \phi \nabla \cdot v - \nabla \cdot alpha \nabla \phi = f $
 * where $ \phi $ is the scalar unknown, $ v $ the convective velocity, $ alpha > 0 $ the diffusivity coefficient,
 * $ f $ the forcing term.
 * Quasi-static algebraic subgrid scale and quasi-static orthogonal subgrid scale methods are exploited for stabilization.
 * The element is designed to use an explicit integration method.
 * @author Riccardo Tosi
 */
template< unsigned int TDim, unsigned int TNumNodes>
class SymbolicQSConvectionDiffusionExplicit : public Element
{
public:
    ///@name Type Definitions
    ///@{

        typedef Element BaseType;
        typedef Node < 3 > NodeType;
        typedef Geometry<NodeType> GeometryType;

    /// Pointer definition of SymbolicQSConvectionDiffusionExplicit
    KRATOS_CLASS_INTRUSIVE_POINTER_DEFINITION(SymbolicQSConvectionDiffusionExplicit);

    ///@}
    ///@name Life Cycle
    ///@{

    //Constructors.
    SymbolicQSConvectionDiffusionExplicit(
        IndexType NewId,
        GeometryType::Pointer pGeometry);
    SymbolicQSConvectionDiffusionExplicit(
        IndexType NewId,
        GeometryType::Pointer pGeometry,
        Properties::Pointer pProperties);

    /// Default constuctor.

    /// Destructor.
    virtual ~SymbolicQSConvectionDiffusionExplicit();

    ///@}
    ///@name Operators
    ///@{


    ///@}
    ///@name Operations
    ///@{

    Element::Pointer Create(
        IndexType NewId,
        NodesArrayType const& ThisNodes,
        Properties::Pointer pProperties) const override;

    Element::Pointer Create(
        IndexType NewId,
        GeometryType::Pointer pGeom,
        Properties::Pointer pProperties) const override;

    void CalculateLocalSystem(
        MatrixType& rLeftHandSideMatrix,
        VectorType& rRightHandSideVector,
        const ProcessInfo& rCurrentProcessInfo) override
    {
        KRATOS_TRY;
        KRATOS_ERROR << "Calling the CalculateLocalSystem() method for the explicit Convection-Diffusion element.";
        KRATOS_CATCH("");
    }

    void CalculateRightHandSide(
        VectorType& rRightHandSideVector,
        const ProcessInfo& rCurrentProcessInfo) override
    {
        KRATOS_TRY;
        KRATOS_ERROR << "Calling the CalculateRightHandSide() method for the explicit Convection-Diffusion element. Call the CalculateRightHandSideInternal() instead.";
        KRATOS_CATCH("");
    }

    void EquationIdVector(
        EquationIdVectorType& rResult,
        const ProcessInfo& rCurrentProcessInfo) const override;

    void GetDofList(
        DofsVectorType& rElementalDofList,
        const ProcessInfo& rCurrentProcessInfo) const override;

    void AddExplicitContribution(
        const ProcessInfo &rCurrentProcessInfo) override;

    void CalculateMassMatrix(
        MatrixType &rMassMatrix,
        const ProcessInfo &rCurrentProcessInfo) override;

    void Calculate(
        const Variable<double>& rVariable,
        double& Output,
        const ProcessInfo& rCurrentProcessInfo) override;

    ///@}
    ///@name Inquiry
    ///@{

    ///@}
    ///@name Input and output
    ///@{

    /// Turn back information as a string.
    std::string Info() const override
    {
        return "SymbolicQSConvectionDiffusionExplicitElement #";
    }

    /// Print information about this object.
    void PrintInfo(std::ostream& rOStream) const override
    {
        rOStream << Info() << Id();
    }

    ///@}

protected:

    ///@name Protected member Variables
    ///@{

    struct ElementData
    {
        // scalars
        double diffusivity;
        double lumping_factor;
        double weight;
        double delta_time;
        double delta_time_coefficient;
        double dynamic_tau;
        double unknown_subscale;
	    double volume;
        // arrays
	    array_1d<double,TNumNodes> tau;
        array_1d<double,TNumNodes> forcing;
        array_1d<double,TNumNodes> unknown;
        array_1d<double,TNumNodes> unknown_old;
        array_1d<double,TNumNodes> oss_projection;
        // matrices
        BoundedMatrix<double,TNumNodes,3> convective_velocity;
        // auxiliary containers for the symbolically-generated matrices
        BoundedMatrix<double,TNumNodes,TNumNodes> lhs;
        array_1d<double,TNumNodes> rhs;
        // auxiliary containers for the symbolically-generated data for Gauss integration
        array_1d<double,TNumNodes> N;
        BoundedMatrix<double,TNumNodes,TNumNodes> N_gausspoint;
	    BoundedMatrix<double,TNumNodes,TDim> DN_DX;
    };

    ///@}
    ///@name Protected Operators
    ///@{


    ///@}
    ///@name Protected Operations
    ///@{

    void InitializeEulerianElement(
        ElementData& rData,
        const ProcessInfo& rCurrentProcessInfo);

    double ComputeH(
        BoundedMatrix<double,TNumNodes,TDim>& rDN_DX);

    ///@}
    ///@name Protected  Access
    ///@{


    ///@}
    ///@name Protected Inquiry
    ///@{

    IntegrationMethod GetIntegrationMethod() const override;

    ///@}
    ///@name Protected LifeCycle
    ///@{

    // Protected default constructor necessary for serialization
    SymbolicQSConvectionDiffusionExplicit() : Element()
    {
    }

    ///@}

private:

    ///@name Static Member Variables
    ///@{

    ///@}
    ///@name Member Variables
    ///@{

    ///@}
    ///@name Serialization
    ///@{

    friend class Serializer;

    void save(Serializer& rSerializer) const override
    {
        KRATOS_SERIALIZE_SAVE_BASE_CLASS(rSerializer, Element);
    }

    void load(Serializer& rSerializer) override
    {
        KRATOS_SERIALIZE_LOAD_BASE_CLASS(rSerializer, Element);
    }

    ///@}
    ///@name Private Operators
    ///@{

    /// Assignment operator
    SymbolicQSConvectionDiffusionExplicit& operator=(SymbolicQSConvectionDiffusionExplicit const& rOther) = delete;

    /// Copy constructor
    SymbolicQSConvectionDiffusionExplicit(SymbolicQSConvectionDiffusionExplicit const& rOther) = delete;

    ///@}
    ///@name Private Operations
    ///@{

    void CalculateRightHandSideInternal(
        BoundedVector<double, TNumNodes>& rRightHandSideBoundedVector,
        const ProcessInfo& rCurrentProcessInfo);

    void CalculateOrthogonalSubgridScaleRHSInternal(
        BoundedVector<double, TNumNodes>& rRightHandSideVector,
        const ProcessInfo& rCurrentProcessInfo);

    void CalculateTau(
        ElementData& rData);

    ///@}
    ///@name Private  Access
    ///@{


    ///@}
    ///@name Private Inquiry
    ///@{


    ///@}
    ///@name Un accessible methods
    ///@{

    ///@}

}; // Class SymbolicQSConvectionDiffusionExplicit

///@}

///@name Type Definitions
///@{


///@}
///@name Input and output
///@{


/// input stream function
template< unsigned int TDim, unsigned int TNumNodes = TDim + 1>
inline std::istream& operator >>(
    std::istream& rIStream,
    SymbolicQSConvectionDiffusionExplicit<TDim,TNumNodes>& rThis)
{
    return rIStream;
}

/// output stream function
template< unsigned int TDim, unsigned int TNumNodes = TDim + 1>
inline std::ostream& operator <<(
    std::ostream& rOStream,
    const SymbolicQSConvectionDiffusionExplicit<TDim,TNumNodes>& rThis)
{
    rThis.PrintInfo(rOStream);
    rOStream << std::endl;
    rThis.PrintData(rOStream);

    return rOStream;
}
///@}

///@}

} // namespace Kratos.

#endif // KRATOS_SYMBOLIC_QS_CONVECTION_DIFFUSION_EXPLICIT_H
