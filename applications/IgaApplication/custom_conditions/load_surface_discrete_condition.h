//    |  /           |
//    ' /   __| _` | __|  _ \   __|
//    . \  |   (   | |   (   |\__ `
//   _|\_\_|  \__,_|\__|\___/ ____/
//                   Multi-Physics
//
//  License:         BSD License
//                   Kratos default license: kratos/license.txt
//
//  Main authors:    Tobias Teschemacher
//


#if !defined(KRATOS_LOAD_SURFACE_DISCRETE_CONDITION_H_INCLUDED )
#define  KRATOS_LOAD_SURFACE_DISCRETE_CONDITION_H_INCLUDED



// System includes

// External includes

// Project includes
#include "includes/define.h"
#include "includes/condition.h"
#include "includes/variables.h"

#include "iga_application_variables.h"
#include "custom_conditions/base_discrete_condition.h"


namespace Kratos
{
    class LoadSurfaceDiscreteCondition
        : public BaseDiscreteCondition
    {
    public:
        /// Counted pointer of LoadSurfaceDiscreteCondition
        KRATOS_CLASS_POINTER_DEFINITION(LoadSurfaceDiscreteCondition);

        /// Default constructor.
        LoadSurfaceDiscreteCondition(IndexType NewId, GeometryType::Pointer pGeometry)
            : BaseDiscreteCondition(NewId, pGeometry)
        {};

        // with properties
        LoadSurfaceDiscreteCondition(IndexType NewId, GeometryType::Pointer pGeometry, PropertiesType::Pointer pProperties)
            : BaseDiscreteCondition(NewId, pGeometry, pProperties)
        {};

        LoadSurfaceDiscreteCondition() : BaseDiscreteCondition()
        {};

        /// Destructor.
        virtual ~LoadSurfaceDiscreteCondition() override
        {};

        Condition::Pointer Create(IndexType NewId, NodesArrayType const& ThisNodes, PropertiesType::Pointer pProperties) const override
        {
            return Kratos::make_shared< LoadSurfaceDiscreteCondition >(NewId, GetGeometry().Create(ThisNodes), pProperties);
        };

        /**
        * @brief Sets on rResult the ID's of the element degrees of freedom
        * @param rResult The vector containing the equation id
        * @param rCurrentProcessInfo The current process info instance
        */
        void EquationIdVector(
            EquationIdVectorType& rResult,
            ProcessInfo& rCurrentProcessInfo
        ) override;

        /**
        * @brief Sets on rElementalDofList the degrees of freedom of the considered element geometry
        * @param rElementalDofList The vector containing the dof of the element
        * @param rCurrentProcessInfo The current process info instance
        */
        void GetDofList(
            DofsVectorType& rElementalDofList,
            ProcessInfo& rCurrentProcessInfo
        ) override;

        /**
        * This functions calculates both the RHS and the LHS
        * @param rLeftHandSideMatrix: The LHS
        * @param rRightHandSideVector: The RHS
        * @param rCurrentProcessInfo: The current process info instance
        * @param CalculateStiffnessMatrixFlag: The flag to set if compute the LHS
        * @param CalculateResidualVectorFlag: The flag to set if compute the RHS
        */
        void CalculateAll(
            MatrixType& rLeftHandSideMatrix,
            VectorType& rRightHandSideVector,
            ProcessInfo& rCurrentProcessInfo,
            const bool CalculateStiffnessMatrixFlag,
            const bool CalculateResidualVectorFlag
        );

        /**
        * This functions calculates the base vector on the location
        * of the condition
        * @param rBaseVector: Base vector on location of the condition
        *                     on the surface.
        */
        void CalculateBaseVector(
            Vector& rBaseVector,
            const Matrix& rDN_De);


        /// Turn back information as a string.
        std::string Info() const override
        {
            std::stringstream buffer;
            buffer << "\"LoadSurfaceDiscreteCondition\" #" << Id();
            return buffer.str();
        }

        /// Print information about this object.
        void PrintInfo(std::ostream& rOStream) const override
        {
            rOStream << "\"LoadSurfaceDiscreteCondition\" #" << Id();
        }

        /// Print object's data.
        void PrintData(std::ostream& rOStream) const {
            pGetGeometry()->PrintData(rOStream);
        }

    private:

        friend class Serializer;

        virtual void save(Serializer& rSerializer) const override
        {
            KRATOS_SERIALIZE_SAVE_BASE_CLASS(rSerializer, BaseDiscreteCondition);
        }

        virtual void load(Serializer& rSerializer) override
        {
            KRATOS_SERIALIZE_LOAD_BASE_CLASS(rSerializer, BaseDiscreteCondition);
        }

    }; // Class LoadSurfaceDiscreteCondition

}  // namespace Kratos.

#endif // KRATOS_LOAD_SURFACE_DISCRETE_CONDITION_H_INCLUDED  defined 