// Project includes
#include "runge_kutta_scheme.h"

namespace Kratos {

    void RungeKuttaScheme::SetIntegrationSchemeInProperties(Properties::Pointer pProp, bool verbose) const {
        if(verbose) std::cout << "\nAssigning RungeKuttaScheme to properties " << pProp->Id() << std::endl;
        pProp->SetValue(DEM_TRANSLATIONAL_INTEGRATION_SCHEME_POINTER, this->CloneShared());
        pProp->SetValue(DEM_ROTATIONAL_INTEGRATION_SCHEME_POINTER, this->CloneShared());
    }

    void RungeKuttaScheme::UpdateTranslationalVariables(
            int StepFlag,
            Node < 3 > & i,
            array_1d<double, 3 >& coor,
            array_1d<double, 3 >& displ,
            array_1d<double, 3 >& delta_displ,
            array_1d<double, 3 >& vel,
            const array_1d<double, 3 >& initial_coor,
            const array_1d<double, 3 >& force,
            const double force_reduction_factor,
            const double mass,
            const double delta_t,
            const bool Fix_vel[3])
    {
        KRATOS_THROW_ERROR(std::runtime_error, "This scheme (RungeKuttaScheme) should not calculate translational motion, so the function (RungeKuttaScheme::UpdateTranslationalVariables) shouldn't be accessed", 0);
    }

    void RungeKuttaScheme::CalculateNewRotationalVariables(
                int StepFlag,
                const Node < 3 > & i,
                double moment_of_inertia,
                array_1d<double, 3 >& angular_velocity,
                array_1d<double, 3 >& torque,
                const double moment_reduction_factor,
                array_1d<double, 3 >& rotated_angle,
                array_1d<double, 3 >& delta_rotation,
                const double delta_t,
                const bool Fix_Ang_vel[3]) {
        
        array_1d<double, 3 >& angular_momentum       = i.FastGetSolutionStepValue(ANGULAR_MOMENTUM);
        Quaternion<double  > Orientation             = Quaternion<double>(1.0, 0.0, 0.0, 0.0);
        array_1d<double, 3 > angular_momentum_aux;
        angular_momentum_aux[0] = 0.0;
        angular_momentum_aux[1] = 0.0;
        angular_momentum_aux[2] = 0.0;

        if (Fix_Ang_vel[0] == true || Fix_Ang_vel[1] == true || Fix_Ang_vel[2] == true) {
            double LocalTensor[3][3], GlobalTensor[3][3];
            array_1d<double, 3 >& moments_of_inertia;
            moments_of_inertia[0] = moment_of_inertia;
            moments_of_inertia[1] = moment_of_inertia;
            moments_of_inertia[2] = moment_of_inertia;
            
            GeometryFunctions::ConstructLocalTensor(moments_of_inertia, LocalTensor);
            GeometryFunctions::QuaternionTensorLocal2Global(Orientation, LocalTensor, GlobalTensor);
            GeometryFunctions::ProductMatrix3X3Vector3X1(GlobalTensor, angular_velocity, angular_momentum_aux);
        }

        if (StepFlag != 1)
        {
            for (int j = 0; j < 3; j++) {
                if (Fix_Ang_vel[j] == false) {
                    angular_momentum[j] += moment_reduction_factor * torque[j] * delta_t;
                }
                else {
                    angular_momentum[j] = angular_momentum_aux[j];
                }
            }
            
            CalculateAngularVelocityRK(Orientation, moments_of_inertia, angular_momentum, angular_velocity, delta_t, Fix_Ang_vel);
            UpdateRotationalVariablesOfCluster(i, moments_of_inertia, rotated_angle, delta_rotation, Orientation, angular_momentum, angular_velocity, delta_t, Fix_Ang_vel);
        }
    }
    
    void RungeKuttaScheme::CalculateNewRotationalVariables(
                int StepFlag,
                const Node < 3 > & i,
                array_1d<double, 3 >& moments_of_inertia,
                array_1d<double, 3 >& angular_velocity,
                array_1d<double, 3 >& torque,
                const double moment_reduction_factor,
                array_1d<double, 3 >& rotated_angle,
                array_1d<double, 3 >& delta_rotation,
                const double delta_t,
                const bool Fix_Ang_vel[3]) {
        
        array_1d<double, 3 >& angular_momentum       = i.FastGetSolutionStepValue(ANGULAR_MOMENTUM);
        array_1d<double, 3 >& local_angular_velocity = i.FastGetSolutionStepValue(LOCAL_ANGULAR_VELOCITY);
        Quaternion<double  >& Orientation            = i.FastGetSolutionStepValue(ORIENTATION);

        array_1d<double, 3 > angular_momentum_aux;
        angular_momentum_aux[0] = 0.0;
        angular_momentum_aux[1] = 0.0;
        angular_momentum_aux[2] = 0.0;

        if (Fix_Ang_vel[0] == true || Fix_Ang_vel[1] == true || Fix_Ang_vel[2] == true) {
            double LocalTensor[3][3], GlobalTensor[3][3];
            array_1d<double, 3 >& moments_of_inertia;
            moments_of_inertia[0] = moment_of_inertia;
            moments_of_inertia[1] = moment_of_inertia;
            moments_of_inertia[2] = moment_of_inertia;
            
            GeometryFunctions::ConstructLocalTensor(moments_of_inertia, LocalTensor);
            GeometryFunctions::QuaternionTensorLocal2Global(Orientation, LocalTensor, GlobalTensor);
            GeometryFunctions::ProductMatrix3X3Vector3X1(GlobalTensor, angular_velocity, angular_momentum_aux);
        }

        if (StepFlag != 1)
        {
            for (int j = 0; j < 3; j++) {
                if (Fix_Ang_vel[j] == false) {
                    angular_momentum[j] += moment_reduction_factor * torque[j] * delta_t;
                }
                else {
                    angular_momentum[j] = angular_momentum_aux[j];
                }
            }
            
            CalculateAngularVelocityRK(Orientation, moments_of_inertia, angular_momentum, angular_velocity, delta_t, Fix_Ang_vel);
            UpdateRotationalVariablesOfCluster(i, moments_of_inertia, rotated_angle, delta_rotation, Orientation, angular_momentum, angular_velocity, delta_t, Fix_Ang_vel);
            GeometryFunctions::QuaternionVectorGlobal2Local(Orientation, angular_velocity, local_angular_velocity);
    }

    void RungeKuttaScheme::UpdateRotationalVariables(
                int StepFlag,
                const Node < 3 > & i,
                array_1d<double, 3 >& rotated_angle,
                array_1d<double, 3 >& delta_rotation,
                array_1d<double, 3 >& angular_velocity,
                array_1d<double, 3 >& angular_acceleration,
                const double delta_t,
                const bool Fix_Ang_vel[3]) {

        for (int k = 0; k < 3; k++) {
            if (Fix_Ang_vel[k] == false) {
                delta_rotation[k] = angular_velocity[k] * delta_t;
                rotated_angle[k] += delta_rotation[k];
                angular_velocity[k] += delta_t * angular_acceleration[k];
            } else {
                delta_rotation[k] = angular_velocity[k] * delta_t;
                rotated_angle[k] += delta_rotation[k];
            }
        }
    }

    void RungeKuttaScheme::UpdateRotationalVariablesOfCluster(
                const Node < 3 > & i,
                const array_1d<double, 3 >& moments_of_inertia,
                array_1d<double, 3 >& rotated_angle,
                array_1d<double, 3 >& delta_rotation,
                Quaternion<double  >& Orientation,
                const array_1d<double, 3 >& angular_momentum,
                array_1d<double, 3 >& angular_velocity,
                const double delta_t,
                const bool Fix_Ang_vel[3]) {

        for (int k = 0; k < 3; k++) {
                delta_rotation[k] = angular_velocity[k] * delta_t;
                rotated_angle[k] += delta_rotation[k];
        }
        
        array_1d<double, 3 > angular_velocity_aux;
        
        double LocalTensorInv[3][3];
        GeometryFunctions::ConstructInvLocalTensor(moments_of_inertia, LocalTensorInv);
        GeometryFunctions::UpdateOrientation(Orientation, delta_rotation);
        UpdateAngularVelocity(Orientation, LocalTensorInv, angular_momentum, angular_velocity_aux);
        for (int j = 0; j < 3; j++) {
            if (Fix_Ang_vel[j] == false){
                angular_velocity[j] = angular_velocity_aux[j];
            }
        }
    }
    
    void RungeKuttaScheme::UpdateRotationalVariables(
                const Node < 3 > & i,
                array_1d<double, 3 >& rotated_angle,
                array_1d<double, 3 >& delta_rotation,
                const array_1d<double, 3 >& angular_velocity,
                const double delta_t,
                const bool Fix_Ang_vel[3]) {
        
        delta_rotation = angular_velocity * delta_t;
        rotated_angle += delta_rotation;
    }
    
    void RungeKuttaScheme::QuaternionCalculateMidAngularVelocities(
                const Quaternion<double>& Orientation,
                const double LocalTensorInv[3][3],
                const array_1d<double, 3>& angular_momentum,
                const double dt,
                const array_1d<double, 3>& InitialAngularVel,
                array_1d<double, 3>& FinalAngularVel) {
        
        array_1d<double, 3 > aux = InitialAngularVel;
        DEM_MULTIPLY_BY_SCALAR_3(aux, dt);
        array_1d<double, 3 > TempDeltaRotation = aux;

        Quaternion<double> TempOrientation;
        double GlobalTensorInv[3][3];
            
        GeometryFunctions::UpdateOrientation(Orientation, TempOrientation, TempDeltaRotation);
        GeometryFunctions::QuaternionTensorLocal2Global(TempOrientation, LocalTensorInv, GlobalTensorInv);
        GeometryFunctions::ProductMatrix3X3Vector3X1(GlobalTensorInv, angular_momentum, FinalAngularVel);
    }
    
    void RungeKuttaScheme::UpdateAngularVelocity(
                const Quaternion<double>& Orientation,
                const double LocalTensorInv[3][3],
                const array_1d<double, 3>& angular_momentum,
                array_1d<double, 3>& angular_velocity) {
        
        double GlobalTensorInv[3][3];
        
        GeometryFunctions::QuaternionTensorLocal2Global(Orientation, LocalTensorInv, GlobalTensorInv);
        GeometryFunctions::ProductMatrix3X3Vector3X1(GlobalTensorInv, angular_momentum, angular_velocity);
    }

    void RungeKuttaScheme::CalculateLocalAngularAcceleration(
                const Node < 3 > & i,
                const double moment_of_inertia,
                const array_1d<double, 3 >& torque,
                const double moment_reduction_factor,
                array_1d<double, 3 >& angular_acceleration) {

        double moment_of_inertia_inv = 1.0 / moment_of_inertia;
        for (int j = 0; j < 3; j++) {
            angular_acceleration[j] = moment_reduction_factor * torque[j] * moment_of_inertia_inv;
        }
    }

    void RungeKuttaScheme::CalculateLocalAngularAccelerationByEulerEquations(
                const Node < 3 > & i,
                const array_1d<double, 3 >& local_angular_velocity,
                const array_1d<double, 3 >& moments_of_inertia,
                const array_1d<double, 3 >& local_torque,
                const double moment_reduction_factor,
                array_1d<double, 3 >& local_angular_acceleration) {

        for (int j = 0; j < 3; j++) {
            local_angular_acceleration[j] = (local_torque[j] - (local_angular_velocity[(j + 1) % 3] * moments_of_inertia[(j + 2) % 3] * local_angular_velocity[(j + 2) % 3] - local_angular_velocity[(j + 2) % 3] * moments_of_inertia[(j + 1) % 3] * local_angular_velocity[(j + 1) % 3])) / moments_of_inertia[j];
            local_angular_acceleration[j] = local_angular_acceleration[j] * moment_reduction_factor;
        }
    }

    void RungeKuttaScheme::CalculateAngularVelocityRK(
                                    const Quaternion<double  >& Orientation,
                                    const array_1d<double, 3 >& moments_of_inertia,
                                    const array_1d<double, 3 >& angular_momentum,
                                    array_1d<double, 3 >& angular_velocity,
                                    const double delta_t,
                                    const bool Fix_Ang_vel[3]) {
            
            double LocalTensorInv[3][3];
            
            GeometryFunctions::ConstructInvLocalTensor(moments_of_inertia, LocalTensorInv);
            
            array_1d<double, 3 > angular_velocity1 = angular_velocity;
            array_1d<double, 3 > angular_velocity2, angular_velocity3, angular_velocity4;

            QuaternionCalculateMidAngularVelocities(Orientation, LocalTensorInv, angular_momentum, 0.5*delta_t, angular_velocity1, angular_velocity2);
            QuaternionCalculateMidAngularVelocities(Orientation, LocalTensorInv, angular_momentum, 0.5*delta_t, angular_velocity2, angular_velocity3);
            QuaternionCalculateMidAngularVelocities(Orientation, LocalTensorInv, angular_momentum,     delta_t, angular_velocity3, angular_velocity4);

            for (int j = 0; j < 3; j++) {
                if (Fix_Ang_vel[j] == false){
                    angular_velocity[j] = 0.16666666666666667 * (angular_velocity1[j] + 2*angular_velocity2[j] + 2*angular_velocity3[j] + angular_velocity4[j]);
                }
            }
    }
} //namespace Kratos
