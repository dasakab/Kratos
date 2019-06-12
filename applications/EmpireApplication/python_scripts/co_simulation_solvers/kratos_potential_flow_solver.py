from __future__ import print_function, absolute_import, division  # makes KratosMultiphysics backward compatible with python 2.6 and 2.7

# Importing the Kratos Library
import KratosMultiphysics
import KratosMultiphysics.CompressiblePotentialFlowApplication
try:
    import KratosMultiphysics.MeshMovingApplication
    KratosMultiphysics.Logger.PrintInfo("MeshMovingApplication", "succesfully imported")
except ImportError:
    KratosMultiphysics.Logger.PrintInfo("MeshMovingApplication", "not imported")

# Importing the base class
from kratos_base_field_solver import KratosBaseFieldSolver

# Other imports
from KratosMultiphysics.CompressiblePotentialFlowApplication.potential_flow_analysis import PotentialFlowAnalysis
from KratosMultiphysics.CompressiblePotentialFlowApplication.compute_forces_on_nodes_process import ComputeForcesOnNodesProcess
from KratosMultiphysics.CompressiblePotentialFlowApplication.define_wake_process_2d import DefineWakeProcess2D
from KratosMultiphysics.CompressiblePotentialFlowApplication.compute_lift_process import ComputeLiftProcess

def CreateSolver(cosim_solver_settings, level):
    return KratosPotentialFlowSolver(cosim_solver_settings, level)

class KratosPotentialFlowSolver(KratosBaseFieldSolver):
    def _CreateAnalysisStage(self):
        return PotentialFlowAnalysis(self.model, self.project_parameters)

    def Initialize(self):

        super(KratosPotentialFlowSolver, self).Initialize()
        model_part = self.model["Body2D_Body"]

        project_parameters = KratosMultiphysics.Parameters("""{
            "model_part_name"                    : "FluidModelPart.Body2D_Body",
            "file_format"                        : "ascii",
            "output_precision"                   : 7,
            "output_control_type"                : "step",
            "output_frequency"                   : 1.0,
            "output_sub_model_parts"             : false,
            "folder_name"                        : "VTK_Output",
            "custom_name_prefix"                 : "",
            "save_output_files_in_folder"        : true,
            "nodal_solution_step_data_variables" : ["MESH_DISPLACEMENT_Y"],
            "nodal_data_value_variables"         : [],
            "nodal_flags"                        : [],
            "element_data_value_variables"       : [],
            "element_flags"                      : [],
            "condition_data_value_variables"     : [],
            "condition_flags"                    : [],
            "gauss_point_variables"              : []
        }""")
        print("LLLLLLLLLL", project_parameters)

        self.vtk_io = KratosMultiphysics.VtkOutput(model_part, project_parameters)
        self.output_frequency = project_parameters["output_frequency"].GetDouble()
        self.output_control = project_parameters["output_control_type"].GetString()
        self.step_count = 0
        self.next_output = model_part.ProcessInfo[KratosMultiphysics.STEP]
        print("YYYYYYYYYYYYYYYYYYYYYYYYYYYYYYY", self.next_output)

        sub_project_parameters = self.project_parameters["processes"]["boundary_conditions_process_list"]

        for i in range(sub_project_parameters.size()):
            if sub_project_parameters[i]["python_module"].GetString() == "define_wake_process_2d":
                self.wake_process = DefineWakeProcess2D(self.model, sub_project_parameters[i]["Parameters"])
                if not hasattr(self, "wake_process"):
                    raise Exception("potential flow requires specification of a process for the wake (currently specifically using 'define_wake_process_2d')")

            if sub_project_parameters[i]["python_module"].GetString() == "compute_forces_on_nodes_process":
                self.conversion_process = ComputeForcesOnNodesProcess(self.model, sub_project_parameters[i]["Parameters"])
            if sub_project_parameters[i]["python_module"].GetString() == "compute_lift_process":
                self.lift_process = ComputeLiftProcess(self.model, sub_project_parameters[i]["Parameters"])

        # self.next_output = self.model_part.ProcessInfo[KratosMultiphysics.STEP]

    def SolveSolutionStep(self):
        self.step_count += 1
        print("YYYYYYYYYYYYYYYYYYYYYYYYYYYYYYY", self.next_output)
        # self.step_count += 1
        self.wake_process._CleanMarking()

        self.wake_process.ExecuteBeforeSolutionLoop()

        super(KratosPotentialFlowSolver, self).SolveSolutionStep()

        self.conversion_process.ExecuteFinalizeSolutionStep()
        self.lift_process.ExecuteFinalizeSolutionStep()

        self.PrintOutput()

    def FinalizeSolutionStep(self):
        super(KratosPotentialFlowSolver, self).FinalizeSolutionStep()

    def _GetParallelType(self):
        return self.project_parameters["problem_data"]["parallel_type"].GetString()

    def _Name(self):
        return self.__class__.__name__

    def PrintOutput(self):
        self.vtk_io.PrintOutput()

        # Schedule next output
        # time = GetPrettyTime(self.model_part.ProcessInfo[KratosMultiphysics.TIME])
        if self.output_frequency > 0.0: # Note: if == 0, we'll just always print
            while self.next_output <= self.step_count:
                self.next_output += self.output_frequency