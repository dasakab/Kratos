//    |  /           |
//    ' /   __| _` | __|  _ \   __|
//    . \  |   (   | |   (   |\__ `
//   _|\_\_|  \__,_|\__|\___/ ____/
//                   Multi-Physics
//
//  License:		 BSD License
//					 Kratos default license: kratos/license.txt
//
//  Main authors:    Suneth Warnakulasuriya (https://github.com/sunethwarna)
//

#if !defined(KRATOS_RANS_EPSILON_TURBULENT_MIXING_LENGTH_EVALUATION_PROCESS_H_INCLUDED)
#define KRATOS_RANS_EPSILON_TURBULENT_MIXING_LENGTH_EVALUATION_PROCESS_H_INCLUDED

// System includes
#include <cmath>
#include <string>

// External includes

// Project includes
#include "custom_utilities/rans_variable_utils.h"
#include "includes/cfd_variables.h"
#include "includes/checks.h"
#include "includes/define.h"
#include "includes/model_part.h"
#include "processes/process.h"
#include "rans_modelling_application_variables.h"

namespace Kratos
{
///@addtogroup RANSModellingApplication
///@{

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

/// Auxiliary process to set Boussinesq buoyancy forces in variable temperature flows.
/** This process modifies the BODY_FORCE variable according to the Boussinesq hypothesis
    so that the fluid element can take natural convection into account.

    This process makes use of the following data:
    - TEMPERATURE from the nodal solution step data: current temperature for the node (mandatory).
    - AMBIENT_TEMPERATURE from ProcessInfo: The reference temperature for the simulation (mandatory).
    - gravity from the Parameters passed in the constructor: an array that defines the gravity vector (mandatory).
    - thermal_expansion_coefficient from the Parameters: a double defining the thermal expansion coefficient for the fluid (optional).

    With this, the process calculates the Boussinesq force and assings it to the BODY_FORCE solution step variable of each node.
    The force is set to (1 + thermal_expansion_coefficient*(temperature - ambient_temperature) ) * g

    If the thermal expansion coefficient is not provided, it is assumed to be (1/ambient_temperature).
    This is the usual value for perfect gases (if the temperature is given in Kelvin).
 */

class RansEpsilonTurbulentMixingLengthEvaluationProcess : public Process
{
public:
    ///@name Type Definitions
    ///@{

    typedef ModelPart::NodeType NodeType;

    typedef ModelPart::NodesContainerType NodesContainerType;

    /// Pointer definition of RansEpsilonTurbulentMixingLengthEvaluationProcess
    KRATOS_CLASS_POINTER_DEFINITION(RansEpsilonTurbulentMixingLengthEvaluationProcess);

    ///@}
    ///@name Life Cycle
    ///@{

    /// Constructor
    RansEpsilonTurbulentMixingLengthEvaluationProcess(ModelPart& rModelPart,
                                                      Parameters& rParameters,
                                                      const bool IsConstrained)
        : mrModelPart(rModelPart), mrParameters(rParameters)
    {
        KRATOS_TRY

        Parameters default_parameters = Parameters(R"(
        {
            "turbulent_mixing_length" : 0.005,
            "c_mu"                    : 0.09
        })");

        mrParameters.RecursivelyValidateAndAssignDefaults(default_parameters);

        mTurbulentMixingLength = mrParameters["turbulent_mixing_length"].GetDouble();
        mIsConstrained = IsConstrained;
        mCmu_75 = std::pow(mrParameters["c_mu"].GetDouble(), 0.75);

        KRATOS_ERROR_IF(mTurbulentMixingLength < std::numeric_limits<double>::epsilon())
            << "turbulent_mixing_length should be greater than zero.\n";

        KRATOS_CATCH("");
    }
    /// Destructor.
    ~RansEpsilonTurbulentMixingLengthEvaluationProcess() override
    {
        // delete mpDistanceCalculator;
    }

    ///@}
    ///@name Operators
    ///@{

    ///@}
    ///@name Operations
    ///@{

    void ExecuteInitializeSolutionStep() override
    {
        Execute();
    }

    void Execute() override
    {
        KRATOS_TRY

        KRATOS_INFO(this->Info())
            << "Applying epsilon values to " << mrModelPart.Name() << ".\n";
        ModelPart::NodesContainerType& r_nodes = mrModelPart.Nodes();
        int number_of_nodes = r_nodes.size();

#pragma omp parallel for
        for (int i_node = 0; i_node < number_of_nodes; ++i_node)
        {
            NodeType& r_node = *(r_nodes.begin() + i_node);
            CalculateTurbulentValues(r_node);
            if (mIsConstrained)
                r_node.Fix(TURBULENT_ENERGY_DISSIPATION_RATE);
        }

        KRATOS_CATCH("");
    }

    int Check() override
    {
        // Checking variable definitions
        KRATOS_CHECK_VARIABLE_KEY(TURBULENT_KINETIC_ENERGY);
        KRATOS_CHECK_VARIABLE_KEY(TURBULENT_ENERGY_DISSIPATION_RATE);

        ModelPart::NodesContainerType& r_nodes = mrModelPart.Nodes();
        int number_of_nodes = r_nodes.size();

#pragma omp parallel for
        for (int i_node = 0; i_node < number_of_nodes; ++i_node)
        {
            NodeType& r_node = *(r_nodes.begin() + i_node);
            KRATOS_CHECK_VARIABLE_IN_NODAL_DATA(TURBULENT_KINETIC_ENERGY, r_node);
            KRATOS_CHECK_VARIABLE_IN_NODAL_DATA(TURBULENT_ENERGY_DISSIPATION_RATE, r_node);
        }

        return 0;
    }

    ///@}
    ///@name Access
    ///@{

    ///@}
    ///@name Inquiry
    ///@{

    ///@}
    ///@name Input and output
    ///@{

    /// Turn back information as a string.
    std::string Info() const override
    {
        return std::string("RansEpsilonTurbulentMixingLengthEvaluationProcess");
    }

    /// Print information about this object.
    void PrintInfo(std::ostream& rOStream) const override
    {
        rOStream << this->Info();
    }

    /// Print object's data.
    void PrintData(std::ostream& rOStream) const override
    {
    }

    ///@}
    ///@name Friends
    ///@{

    ///@}

protected:
    ///@name Protected static Member Variables
    ///@{

    ///@}
    ///@name Protected member Variables
    ///@{

    ///@}
    ///@name Protected Operators
    ///@{

    ///@}
    ///@name Protected Operations
    ///@{

    ///@}
    ///@name Protected  Access
    ///@{

    ///@}
    ///@name Protected Inquiry
    ///@{

    ///@}
    ///@name Protected LifeCycle
    ///@{

    ///@}

private:
    ///@name Static Member Variables
    ///@{

    ///@}
    ///@name Member Variables
    ///@{

    ModelPart& mrModelPart;
    Parameters& mrParameters;

    double mTurbulentMixingLength;
    double mCmu_75;
    bool mIsConstrained;

    ///@}
    ///@name Private Operators
    ///@{

    ///@}
    ///@name Private Operations
    ///@{

    void CalculateTurbulentValues(NodeType& rNode)
    {
        const double tke = rNode.FastGetSolutionStepValue(TURBULENT_KINETIC_ENERGY);
        rNode.FastGetSolutionStepValue(TURBULENT_ENERGY_DISSIPATION_RATE) =
            mCmu_75 * std::pow(std::max(tke, 0.0), 1.5) / mTurbulentMixingLength;
    }

    ///@}
    ///@name Private  Access
    ///@{

    ///@}
    ///@name Private Inquiry
    ///@{

    ///@}
    ///@name Un accessible methods
    ///@{

    /// Assignment operator.
    RansEpsilonTurbulentMixingLengthEvaluationProcess& operator=(
        RansEpsilonTurbulentMixingLengthEvaluationProcess const& rOther);

    /// Copy constructor.
    RansEpsilonTurbulentMixingLengthEvaluationProcess(
        RansEpsilonTurbulentMixingLengthEvaluationProcess const& rOther);

    ///@}

}; // Class RansEpsilonTurbulentMixingLengthEvaluationProcess

///@}

///@name Type Definitions
///@{

///@}
///@name Input and output
///@{

/// output stream function
inline std::ostream& operator<<(std::ostream& rOStream,
                                const RansEpsilonTurbulentMixingLengthEvaluationProcess& rThis);

///@}

///@} addtogroup block

} // namespace Kratos.

#endif // KRATOS_RANS_EPSILON_TURBULENT_MIXING_LENGTH_EVALUATION_PROCESS_H_INCLUDED defined
