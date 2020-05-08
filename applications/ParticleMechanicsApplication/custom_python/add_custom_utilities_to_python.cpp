//    |  /           |
//    ' /   __| _` | __|  _ \   __|
//    . \  |   (   | |   (   |\__ \.
//   _|\_\_|  \__,_|\__|\___/ ____/
//                   Multi-Physics
//
//  License:		 BSD License
//					 Kratos default license: kratos/license.txt
//
//  Main authors:    Ilaria Iaconeta, Bodhinanda Chandra
//
//


// System includes

// External includes

// Project includes
#include "includes/define.h"
#include "processes/process.h"
#include "custom_python/add_custom_utilities_to_python.h"
#include "spaces/ublas_space.h"
#include "linear_solvers/linear_solver.h"
#include "custom_utilities/mpm_search_element_utility.h"
#include "custom_utilities/mpm_particle_generator_utility.h"
#include "custom_utilities/mpm_temporal_coupling_utility.h"


namespace Kratos{
namespace Python{

    void SearchElementAccordingToDimension(ModelPart& rBackgroundGridModelPart, ModelPart& rMPMModelPart, const std::size_t MaxNumberOfResults,
        const double Tolerance)
    {
        const auto dimension = rBackgroundGridModelPart.GetProcessInfo()[DOMAIN_SIZE];
        if (dimension == 2) MPMSearchElementUtility::SearchElement<2>(rBackgroundGridModelPart, rMPMModelPart, MaxNumberOfResults, Tolerance);
        else if (dimension == 3) MPMSearchElementUtility::SearchElement<3>(rBackgroundGridModelPart, rMPMModelPart, MaxNumberOfResults, Tolerance);
    }

    void  AddCustomUtilitiesToPython(pybind11::module& m)
    {
        m.def("SearchElement", SearchElementAccordingToDimension);
        m.def("GenerateMaterialPointElement", &MPMParticleGeneratorUtility::GenerateMaterialPointElement);
        m.def("GenerateMaterialPointCondition", &MPMParticleGeneratorUtility::GenerateMaterialPointCondition);

        pybind11::class_< MPMTemporalCouplingUtility>(m, "MPMTemporalCouplingUtility")
            .def(pybind11::init<ModelPart&, ModelPart&, ModelPart&, unsigned int, double, double, double>())
            .def("CalculateCorrectiveLagrangianMultipliers", &MPMTemporalCouplingUtility::CalculateCorrectiveLagrangianMultipliers)
            .def("InitializeSubDomain1Coupling", &MPMTemporalCouplingUtility::InitializeSubDomain1Coupling)
            .def("StoreFreeVelocitiesSubDomain1", &MPMTemporalCouplingUtility::StoreFreeVelocitiesSubDomain1)
            .def("CorrectSubDomain1", &MPMTemporalCouplingUtility::CorrectSubDomain1)
            ;
    }

}  // namespace Python.
} // Namespace Kratos

