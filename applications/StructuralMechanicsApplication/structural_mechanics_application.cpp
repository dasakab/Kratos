// KRATOS  ___|  |                   |                   |
//       \___ \  __|  __| |   |  __| __| |   |  __| _` | |
//             | |   |    |   | (    |   |   | |   (   | |
//       _____/ \__|_|   \__,_|\___|\__|\__,_|_|  \__,_|_| MECHANICS
//
//  License:		 BSD License
//					 license: structural_mechanics_application/license.txt
//
//  Main authors:    Riccardo Rossi
//    Co-authors:    Vicente Mataix Ferrándiz
//

// System includes

// External includes

// Project includes
#include "includes/define.h"

#include "structural_mechanics_application_variables.h"
#include "structural_mechanics_application.h"
#include "includes/variables.h"
#include "includes/constitutive_law.h"

#include "geometries/triangle_3d_3.h"
#include "geometries/triangle_3d_6.h"
#include "geometries/quadrilateral_3d_4.h"
#include "geometries/quadrilateral_3d_8.h"
#include "geometries/quadrilateral_3d_9.h"
#include "geometries/prism_3d_6.h"
#include "geometries/prism_3d_15.h"
#include "geometries/tetrahedra_3d_4.h"
#include "geometries/tetrahedra_3d_10.h"
#include "geometries/hexahedra_3d_8.h"
#include "geometries/hexahedra_3d_20.h"
#include "geometries/hexahedra_3d_27.h"

#include "geometries/line_2d_2.h"
#include "geometries/line_2d_3.h"
#include "geometries/line_3d_2.h"
#include "geometries/line_3d_3.h"
#include "geometries/point_2d.h"
#include "geometries/point_3d.h"
#include "geometries/triangle_2d_3.h"
#include "geometries/triangle_2d_6.h"
#include "geometries/quadrilateral_2d_4.h"
#include "geometries/quadrilateral_2d_8.h"
#include "geometries/quadrilateral_2d_9.h"

namespace Kratos {

    // We define the node type
    typedef Node<3> NodeType;

KratosStructuralMechanicsApplication::KratosStructuralMechanicsApplication()
    : KratosApplication("StructuralMechanicsApplication"),
      /* ELEMENTS */
      // Adding the truss elements
      mTrussElement3D2N(0, Element::GeometryType::Pointer(new Line3D2<NodeType >(Element::GeometryType::PointsArrayType(2)))),
      mTrussLinearElement3D2N(0, Element::GeometryType::Pointer(new Line3D2<NodeType >(Element::GeometryType::PointsArrayType(2)))),
      mCableElement3D2N(0, Element::GeometryType::Pointer(new Line3D2<NodeType >(Element::GeometryType::PointsArrayType(2)))),
      // Adding the beam elements
      mCrBeamElement3D2N(0, Element::GeometryType::Pointer(new Line3D2<NodeType >(Element::GeometryType::PointsArrayType(2)))),
      mCrLinearBeamElement3D2N(0, Element::GeometryType::Pointer(new Line3D2<NodeType >(Element::GeometryType::PointsArrayType(2)))),
      mCrBeamElement2D2N(0, Element::GeometryType::Pointer(new Line2D2<NodeType >(Element::GeometryType::PointsArrayType(2)))),
      mCrLinearBeamElement2D2N(0, Element::GeometryType::Pointer(new Line2D2<NodeType >(Element::GeometryType::PointsArrayType(2)))),
      // Adding the shells elements
      mIsotropicShellElement3D3N(0, Element::GeometryType::Pointer(new Triangle3D3<NodeType >(Element::GeometryType::PointsArrayType(3)))),
      mShellThickElement3D4N(0, Element::GeometryType::Pointer(new Quadrilateral3D4<NodeType >(Element::GeometryType::PointsArrayType(4))), false),
      mShellThickCorotationalElement3D4N(0, Element::GeometryType::Pointer(new Quadrilateral3D4<NodeType >(Element::GeometryType::PointsArrayType(4))), true),
      mShellThinCorotationalElement3D4N(0, Element::GeometryType::Pointer(new Quadrilateral3D4<NodeType >(Element::GeometryType::PointsArrayType(4))), true),
      mShellThinElement3D3N(0, Element::GeometryType::Pointer(new Triangle3D3<NodeType >(Element::GeometryType::PointsArrayType(3))), false),
      mShellThinCorotationalElement3D3N(0, Element::GeometryType::Pointer(new Triangle3D3<NodeType >(Element::GeometryType::PointsArrayType(3))), true),
      mShellThickCorotationalElement3D3N(0, Element::GeometryType::Pointer(new Triangle3D3<NodeType >(Element::GeometryType::PointsArrayType(3))), true),
      // Adding the membrane element
      mPreStressMembraneElement3D3N(0, Element::GeometryType::Pointer(new Triangle3D3<NodeType >(Element::GeometryType::PointsArrayType(3)))),
      mPreStressMembraneElement3D4N(0, Element::GeometryType::Pointer(new Quadrilateral3D4<NodeType >(Element::GeometryType::PointsArrayType(4)))),
      // Adding the SPRISM element
      mSolidShellElementSprism3D6N(0, Element::GeometryType::Pointer(new Prism3D6<NodeType >(Element::GeometryType::PointsArrayType(6)))),
      // Adding the nodal concentrated element
      mNodalConcentratedElement2D1N(0, Element::GeometryType::Pointer(new Point2D<NodeType >(Element::GeometryType::PointsArrayType(1))), true),
      mNodalConcentratedDampedElement2D1N(0, Element::GeometryType::Pointer(new Point2D<NodeType >(Element::GeometryType::PointsArrayType(1))), false),
      mNodalConcentratedElement3D1N(0, Element::GeometryType::Pointer(new Point3D<NodeType >(Element::GeometryType::PointsArrayType(1))), true),
      mNodalConcentratedDampedElement3D1N(0, Element::GeometryType::Pointer(new Point3D<NodeType >(Element::GeometryType::PointsArrayType(1))), false),
      // Adding the kinematic linear elements
      mSmallDisplacement2D3N(0, Element::GeometryType::Pointer(new Triangle2D3<NodeType >(Element::GeometryType::PointsArrayType(3)))),
      mSmallDisplacement2D4N(0, Element::GeometryType::Pointer(new Quadrilateral2D4<NodeType >(Element::GeometryType::PointsArrayType(4)))),
      mSmallDisplacement2D6N(0, Element::GeometryType::Pointer(new Triangle2D6<NodeType >(Element::GeometryType::PointsArrayType(6)))),
      mSmallDisplacement2D8N(0, Element::GeometryType::Pointer(new Quadrilateral2D8<NodeType >(Element::GeometryType::PointsArrayType(8)))),
      mSmallDisplacement2D9N(0, Element::GeometryType::Pointer(new Quadrilateral2D9<NodeType >(Element::GeometryType::PointsArrayType(9)))),
      mSmallDisplacement3D4N(0, Element::GeometryType::Pointer(new Tetrahedra3D4<NodeType >(Element::GeometryType::PointsArrayType(4)))),
      mSmallDisplacement3D6N(0, Element::GeometryType::Pointer(new Prism3D6<NodeType >(Element::GeometryType::PointsArrayType(6)))),
      mSmallDisplacement3D8N(0, Element::GeometryType::Pointer(new Hexahedra3D8<NodeType >(Element::GeometryType::PointsArrayType(8)))),
      mSmallDisplacement3D10N(0, Element::GeometryType::Pointer(new Tetrahedra3D10<NodeType >(Element::GeometryType::PointsArrayType(10)))),
      mSmallDisplacement3D15N(0, Element::GeometryType::Pointer(new Prism3D15<NodeType >(Element::GeometryType::PointsArrayType(15)))),
      mSmallDisplacement3D20N(0, Element::GeometryType::Pointer(new Hexahedra3D20<NodeType >(Element::GeometryType::PointsArrayType(20)))),
      mSmallDisplacement3D27N(0, Element::GeometryType::Pointer(new Hexahedra3D27<NodeType >(Element::GeometryType::PointsArrayType(27)))),

      mSmallDisplacementBbar2D4N(0, Element::GeometryType::Pointer(new Quadrilateral2D4<NodeType>(Element::GeometryType::PointsArrayType(4)))),
      mSmallDisplacementBbar3D8N(0, Element::GeometryType::Pointer(new Hexahedra3D8<NodeType>(Element::GeometryType::PointsArrayType(8)))),

      mAxisymSmallDisplacement2D3N(0, Element::GeometryType::Pointer(new Triangle2D3<NodeType >(Element::GeometryType::PointsArrayType(3)))),
      mAxisymSmallDisplacement2D4N(0, Element::GeometryType::Pointer(new Quadrilateral2D4<NodeType >(Element::GeometryType::PointsArrayType(4)))),
      mAxisymSmallDisplacement2D6N(0, Element::GeometryType::Pointer(new Triangle2D6<NodeType >(Element::GeometryType::PointsArrayType(6)))),
      mAxisymSmallDisplacement2D8N(0, Element::GeometryType::Pointer(new Quadrilateral2D8<NodeType >(Element::GeometryType::PointsArrayType(8)))),
      mAxisymSmallDisplacement2D9N(0, Element::GeometryType::Pointer(new Quadrilateral2D9<NodeType >(Element::GeometryType::PointsArrayType(9)))),

      // Adding the Total lagrangian elements
      mTotalLagrangian2D3N(0, Element::GeometryType::Pointer(new Triangle2D3<NodeType >(Element::GeometryType::PointsArrayType(3)))),
      mTotalLagrangian2D4N(0, Element::GeometryType::Pointer(new Quadrilateral2D4<NodeType >(Element::GeometryType::PointsArrayType(4)))),
      mTotalLagrangian2D6N(0, Element::GeometryType::Pointer(new Triangle2D6<NodeType >(Element::GeometryType::PointsArrayType(6)))),
      mTotalLagrangian2D8N(0, Element::GeometryType::Pointer(new Quadrilateral2D8<NodeType >(Element::GeometryType::PointsArrayType(8)))),
      mTotalLagrangian2D9N(0, Element::GeometryType::Pointer(new Quadrilateral2D9<NodeType >(Element::GeometryType::PointsArrayType(9)))),
      mTotalLagrangian3D4N(0, Element::GeometryType::Pointer(new Tetrahedra3D4<NodeType >(Element::GeometryType::PointsArrayType(4)))),
      mTotalLagrangian3D6N(0, Element::GeometryType::Pointer(new Prism3D6<NodeType >(Element::GeometryType::PointsArrayType(6)))),
      mTotalLagrangian3D8N(0, Element::GeometryType::Pointer(new Hexahedra3D8<NodeType >(Element::GeometryType::PointsArrayType(8)))),
      mTotalLagrangian3D10N(0, Element::GeometryType::Pointer(new Tetrahedra3D10<NodeType >(Element::GeometryType::PointsArrayType(10)))),
      mTotalLagrangian3D15N(0, Element::GeometryType::Pointer(new Prism3D15<NodeType >(Element::GeometryType::PointsArrayType(15)))),
      mTotalLagrangian3D20N(0, Element::GeometryType::Pointer(new Hexahedra3D20<NodeType >(Element::GeometryType::PointsArrayType(20)))),
      mTotalLagrangian3D27N(0, Element::GeometryType::Pointer(new Hexahedra3D27<NodeType >(Element::GeometryType::PointsArrayType(27)))),
      mAxisymTotalLagrangian2D3N(0, Element::GeometryType::Pointer(new Triangle2D3<NodeType >(Element::GeometryType::PointsArrayType(3)))),
      mAxisymTotalLagrangian2D4N(0, Element::GeometryType::Pointer(new Quadrilateral2D4<NodeType >(Element::GeometryType::PointsArrayType(4)))),
      mAxisymTotalLagrangian2D6N(0, Element::GeometryType::Pointer(new Triangle2D6<NodeType >(Element::GeometryType::PointsArrayType(6)))),
      mAxisymTotalLagrangian2D8N(0, Element::GeometryType::Pointer(new Quadrilateral2D8<NodeType >(Element::GeometryType::PointsArrayType(8)))),
      mAxisymTotalLagrangian2D9N(0, Element::GeometryType::Pointer(new Quadrilateral2D9<NodeType >(Element::GeometryType::PointsArrayType(9)))),
      // Adding the Updated lagrangian elements
      mUpdatedLagrangian2D3N(0, Element::GeometryType::Pointer(new Triangle2D3<NodeType >(Element::GeometryType::PointsArrayType(3)))),
      mUpdatedLagrangian2D4N(0, Element::GeometryType::Pointer(new Quadrilateral2D4<NodeType >(Element::GeometryType::PointsArrayType(4)))),
      mUpdatedLagrangian2D6N(0, Element::GeometryType::Pointer(new Triangle2D6<NodeType >(Element::GeometryType::PointsArrayType(6)))),
      mUpdatedLagrangian2D8N(0, Element::GeometryType::Pointer(new Quadrilateral2D8<NodeType >(Element::GeometryType::PointsArrayType(8)))),
      mUpdatedLagrangian2D9N(0, Element::GeometryType::Pointer(new Quadrilateral2D9<NodeType >(Element::GeometryType::PointsArrayType(9)))),
      mUpdatedLagrangian3D4N(0, Element::GeometryType::Pointer(new Tetrahedra3D4<NodeType >(Element::GeometryType::PointsArrayType(4)))),
      mUpdatedLagrangian3D6N(0, Element::GeometryType::Pointer(new Prism3D6<NodeType >(Element::GeometryType::PointsArrayType(6)))),
      mUpdatedLagrangian3D8N( 0, Element::GeometryType::Pointer(new Hexahedra3D8<NodeType >(Element::GeometryType::PointsArrayType(8)))),
      mUpdatedLagrangian3D10N(0, Element::GeometryType::Pointer(new Tetrahedra3D10<NodeType >(Element::GeometryType::PointsArrayType(10)))),
      mUpdatedLagrangian3D15N(0, Element::GeometryType::Pointer(new Prism3D15<NodeType >(Element::GeometryType::PointsArrayType(15)))),
      mUpdatedLagrangian3D20N(0, Element::GeometryType::Pointer(new Hexahedra3D20<NodeType >(Element::GeometryType::PointsArrayType(20)))),
      mUpdatedLagrangian3D27N(0, Element::GeometryType::Pointer(new Hexahedra3D27<NodeType >(Element::GeometryType::PointsArrayType(27)))),
      mAxisymUpdatedLagrangian2D3N(0, Element::GeometryType::Pointer(new Triangle2D3<NodeType >(Element::GeometryType::PointsArrayType(3)))),
      mAxisymUpdatedLagrangian2D4N(0, Element::GeometryType::Pointer(new Quadrilateral2D4<NodeType >(Element::GeometryType::PointsArrayType(4)))),
      mAxisymUpdatedLagrangian2D6N(0, Element::GeometryType::Pointer(new Triangle2D6<NodeType >(Element::GeometryType::PointsArrayType(6)))),
      mAxisymUpdatedLagrangian2D8N(0, Element::GeometryType::Pointer(new Quadrilateral2D8<NodeType >(Element::GeometryType::PointsArrayType(8)))),
      mAxisymUpdatedLagrangian2D9N(0, Element::GeometryType::Pointer(new Quadrilateral2D9<NodeType >(Element::GeometryType::PointsArrayType(9)))),
      // Adding the spring damper element
      mSpringDamperElement3D2N(0, Element::GeometryType::Pointer(new Line3D2<Node<3> >(Element::GeometryType::PointsArrayType(2)))),
      // Addint the adjoint elements
      mShellThinAdjointElement3D3N( 0, Element::GeometryType::Pointer( new Triangle3D3 <Node<3> >( Element::GeometryType::PointsArrayType( 3 ) ) ), false ),
      mCrLinearBeamAdjointElement3D2N( 0, Element::GeometryType::Pointer( new Line3D2 <Node<3> >( Element::GeometryType::PointsArrayType( 2 ) ) ) ),
      /* CONDITIONS */
      // Adding point load conditions
      mPointLoadCondition2D1N(0, Condition::GeometryType::Pointer(new Point2D<NodeType >(Condition::GeometryType::PointsArrayType(1)))),
      mPointLoadCondition3D1N(0, Condition::GeometryType::Pointer(new Point3D<NodeType >(Condition::GeometryType::PointsArrayType(1)))),
      mPointContactCondition2D1N(0, Condition::GeometryType::Pointer(new Point2D<NodeType >(Condition::GeometryType::PointsArrayType(1)))),
      mPointContactCondition3D1N(0, Condition::GeometryType::Pointer(new Point3D<NodeType >(Condition::GeometryType::PointsArrayType(1)))),
      mAxisymPointLoadCondition2D1N(0, Condition::GeometryType::Pointer(new Point2D<NodeType >(Condition::GeometryType::PointsArrayType(1)))),
      // Adding line load conditions
      mLineLoadCondition2D2N(0, Condition::GeometryType::Pointer(new Line2D2<NodeType >(Condition::GeometryType::PointsArrayType(2)))),
      mLineLoadCondition2D3N(0, Condition::GeometryType::Pointer(new Line2D3<NodeType >(Condition::GeometryType::PointsArrayType(3)))),
      mAxisymLineLoadCondition2D2N(0, Condition::GeometryType::Pointer(new Line2D2<NodeType >(Condition::GeometryType::PointsArrayType(2)))),
      mAxisymLineLoadCondition2D3N(0, Condition::GeometryType::Pointer(new Line2D3<NodeType >(Condition::GeometryType::PointsArrayType(3)))),
      // Adding surface load conditions
      mSurfaceLoadCondition3D3N(0, Condition::GeometryType::Pointer(new Triangle3D3<Node<3> >(Condition::GeometryType::PointsArrayType(3)))),
      mSurfaceLoadCondition3D4N(0, Condition::GeometryType::Pointer(new Quadrilateral3D4<Node<3> >( Condition::GeometryType::PointsArrayType(4)))),
      mSurfaceLoadCondition3D6N(0, Condition::GeometryType::Pointer(new Triangle3D6<Node<3> >(Condition::GeometryType::PointsArrayType(6)))),
      mSurfaceLoadCondition3D8N(0, Condition::GeometryType::Pointer(new Quadrilateral3D8<Node<3> >(Condition::GeometryType::PointsArrayType(8)))),
      mSurfaceLoadCondition3D9N(0, Condition::GeometryType::Pointer(new Quadrilateral3D9<Node<3> >(Condition::GeometryType::PointsArrayType(9)))),

      // Adding point moment conditions
      mPointMomentCondition3D1N(0, Condition::GeometryType::Pointer(new Point3D<Node<3> >(Condition::GeometryType::PointsArrayType(1)))),

      // Adding adjoint conditions
      mPointLoadAdjointCondition2D1N(  0, Condition::GeometryType::Pointer( new Point2D <Node<3> >( Condition::GeometryType::PointsArrayType( 1 ) ) ) ),
      mPointLoadAdjointCondition3D1N(  0, Condition::GeometryType::Pointer( new Point3D <Node<3> >( Condition::GeometryType::PointsArrayType( 1 ) ) ) ) {}

void KratosStructuralMechanicsApplication::Register() {
    // calling base class register to register Kratos components
    KratosApplication::Register();

    KRATOS_INFO("") << "     KRATOS   ___|  |                   |                   |                     " << std::endl
                    << "            \\___ \\  __|  __| |   |  __| __| |   |  __| _` | |                   " << std::endl
                    << "                  | |   |    |   | (    |   |   | |   (   | |                     " << std::endl
                    << "            _____/ \\__|_|   \\__,_|\\___|\\__|\\__,_|_|  \\__,_|_| MECHANICS     " << std::endl;

    // Generalized eigenvalue problem
    KRATOS_REGISTER_VARIABLE(BUILD_LEVEL)
    KRATOS_REGISTER_VARIABLE(EIGENVALUE_VECTOR)
    KRATOS_REGISTER_VARIABLE(EIGENVECTOR_MATRIX)

    // Geometrical
    KRATOS_REGISTER_VARIABLE(AREA)
    KRATOS_REGISTER_VARIABLE(IT)
    KRATOS_REGISTER_VARIABLE(IY)
    KRATOS_REGISTER_VARIABLE(IZ)
    KRATOS_REGISTER_VARIABLE(CROSS_AREA)
    KRATOS_REGISTER_VARIABLE(MEAN_RADIUS)
    KRATOS_REGISTER_VARIABLE(SECTION_SIDES)
    KRATOS_REGISTER_VARIABLE(GEOMETRIC_STIFFNESS)
    KRATOS_REGISTER_VARIABLE(LOCAL_ELEMENT_ORIENTATION)
    KRATOS_REGISTER_VARIABLE(MATERIAL_ORIENTATION_ANGLE)
    KRATOS_REGISTER_VARIABLE(LOCAL_MATERIAL_ORIENTATION_VECTOR_1)
    KRATOS_REGISTER_VARIABLE(USE_CONSISTENT_MASS_MATRIX)
    KRATOS_REGISTER_VARIABLE(CONDENSED_DOF_LIST)

    // Truss generalized variables
    KRATOS_REGISTER_VARIABLE(TRUSS_PRESTRESS_PK2)
    KRATOS_REGISTER_VARIABLE(HARDENING_MODULUS_1D)
    KRATOS_REGISTER_VARIABLE(TANGENT_MODULUS)
    KRATOS_REGISTER_VARIABLE(PLASTIC_ALPHA)

    // Beam generalized variables
    KRATOS_REGISTER_VARIABLE(AREA_EFFECTIVE_Y)
    KRATOS_REGISTER_VARIABLE(AREA_EFFECTIVE_Z)
    KRATOS_REGISTER_VARIABLE(INERTIA_ROT_Y)
    KRATOS_REGISTER_VARIABLE(INERTIA_ROT_Z)
    KRATOS_REGISTER_VARIABLE(LOCAL_AXES_VECTOR)
    KRATOS_REGISTER_VARIABLE(TORSIONAL_INERTIA)
    KRATOS_REGISTER_VARIABLE(I22)
    KRATOS_REGISTER_VARIABLE(I33)

    //  Shell generalized variables
    KRATOS_REGISTER_VARIABLE(STENBERG_SHEAR_STABILIZATION_SUITABLE)
    KRATOS_REGISTER_VARIABLE(SHELL_STRAIN)
    KRATOS_REGISTER_VARIABLE(SHELL_FORCE)
    KRATOS_REGISTER_VARIABLE(SHELL_STRAIN_GLOBAL)
    KRATOS_REGISTER_VARIABLE(SHELL_FORCE_GLOBAL)
    KRATOS_REGISTER_VARIABLE(SHELL_CURVATURE)
    KRATOS_REGISTER_VARIABLE(SHELL_CURVATURE_GLOBAL)
    KRATOS_REGISTER_VARIABLE(SHELL_MOMENT)
    KRATOS_REGISTER_VARIABLE(SHELL_MOMENT_GLOBAL)
    KRATOS_REGISTER_VARIABLE(SHELL_STRESS_TOP_SURFACE)
    KRATOS_REGISTER_VARIABLE(SHELL_STRESS_TOP_SURFACE_GLOBAL)
    KRATOS_REGISTER_VARIABLE(SHELL_STRESS_MIDDLE_SURFACE)
    KRATOS_REGISTER_VARIABLE(SHELL_STRESS_MIDDLE_SURFACE_GLOBAL)
    KRATOS_REGISTER_VARIABLE(SHELL_STRESS_BOTTOM_SURFACE)
    KRATOS_REGISTER_VARIABLE(SHELL_STRESS_BOTTOM_SURFACE_GLOBAL)
    KRATOS_REGISTER_VARIABLE(VON_MISES_STRESS_TOP_SURFACE)
    KRATOS_REGISTER_VARIABLE(VON_MISES_STRESS_MIDDLE_SURFACE)
    KRATOS_REGISTER_VARIABLE(VON_MISES_STRESS_BOTTOM_SURFACE)
    KRATOS_REGISTER_VARIABLE(SHELL_ORTHOTROPIC_STRESS_BOTTOM_SURFACE)
    KRATOS_REGISTER_VARIABLE(SHELL_ORTHOTROPIC_STRESS_TOP_SURFACE)
    KRATOS_REGISTER_VARIABLE(SHELL_ORTHOTROPIC_STRESS_BOTTOM_SURFACE_GLOBAL)
    KRATOS_REGISTER_VARIABLE(SHELL_ORTHOTROPIC_STRESS_TOP_SURFACE_GLOBAL)
    KRATOS_REGISTER_VARIABLE(SHELL_ORTHOTROPIC_4PLY_THROUGH_THICKNESS)
    KRATOS_REGISTER_VARIABLE(TSAI_WU_RESERVE_FACTOR)
    KRATOS_REGISTER_VARIABLE(SHELL_ORTHOTROPIC_LAMINA_STRENGTHS)

    // Shell energies
    KRATOS_REGISTER_VARIABLE(SHELL_ELEMENT_MEMBRANE_ENERGY)
    KRATOS_REGISTER_VARIABLE(SHELL_ELEMENT_BENDING_ENERGY)
    KRATOS_REGISTER_VARIABLE(SHELL_ELEMENT_SHEAR_ENERGY)
    KRATOS_REGISTER_VARIABLE(SHELL_ELEMENT_MEMBRANE_ENERGY_FRACTION)
    KRATOS_REGISTER_VARIABLE(SHELL_ELEMENT_BENDING_ENERGY_FRACTION)
    KRATOS_REGISTER_VARIABLE(SHELL_ELEMENT_SHEAR_ENERGY_FRACTION)

    // Prestresse membrane generalized vairiables
    KRATOS_REGISTER_VARIABLE( MEMBRANE_PRESTRESS )
    KRATOS_REGISTER_VARIABLE( PRESTRESS_VECTOR )
    KRATOS_REGISTER_VARIABLE( PRESTRESS_AXIS_1_GLOBAL )
    KRATOS_REGISTER_VARIABLE( PRESTRESS_AXIS_2_GLOBAL )
    KRATOS_REGISTER_VARIABLE( PRESTRESS_AXIS_1 )
    KRATOS_REGISTER_VARIABLE( PRESTRESS_AXIS_2 )
    KRATOS_REGISTER_VARIABLE( PROJECTION_TYPE_COMBO )

    // Formfinding
    KRATOS_REGISTER_VARIABLE(LAMBDA_MAX)
    KRATOS_REGISTER_VARIABLE(IS_FORMFINDING)
    KRATOS_REGISTER_VARIABLE(BASE_REF_1)
    KRATOS_REGISTER_VARIABLE(BASE_REF_2)
    


    // Cross section
    KRATOS_REGISTER_VARIABLE(SHELL_CROSS_SECTION)
    KRATOS_REGISTER_VARIABLE(SHELL_CROSS_SECTION_OUTPUT_PLY_ID)
    KRATOS_REGISTER_VARIABLE(SHELL_CROSS_SECTION_OUTPUT_PLY_LOCATION)
    KRATOS_REGISTER_VARIABLE(SHELL_ORTHOTROPIC_LAYERS)

    // Nodal stiffness
    KRATOS_REGISTER_3D_VARIABLE_WITH_COMPONENTS(NODAL_STIFFNESS)
    KRATOS_REGISTER_3D_VARIABLE_WITH_COMPONENTS(NODAL_DAMPING_RATIO)

    // For explicit central difference scheme
    KRATOS_REGISTER_3D_VARIABLE_WITH_COMPONENTS(MIDDLE_VELOCITY)
    KRATOS_REGISTER_3D_VARIABLE_WITH_COMPONENTS(MIDDLE_ANGULAR_VELOCITY)
    KRATOS_REGISTER_3D_VARIABLE_WITH_COMPONENTS(NODAL_INERTIA)

    // CONDITIONS
    /* Moment condition */
    KRATOS_REGISTER_3D_VARIABLE_WITH_COMPONENTS(POINT_MOMENT)

    // Adding the SPRISM EAS variables
    KRATOS_REGISTER_VARIABLE(ALPHA_EAS);
    KRATOS_REGISTER_VARIABLE(EAS_IMP);
    KRATOS_REGISTER_VARIABLE(SPRISM_TL_UL);

    // Adding the SPRISM additional variables
    KRATOS_REGISTER_VARIABLE(ANG_ROT);

    // Adding the SPRISM number of transversal integration points
    KRATOS_REGISTER_VARIABLE(NINT_TRANS);

    // Adding the SPRISM variable to deactivate the quadratic interpolation
    KRATOS_REGISTER_VARIABLE(QUAD_ON);

    // Strain measures
    KRATOS_REGISTER_VARIABLE(HENCKY_STRAIN_VECTOR);
    KRATOS_REGISTER_VARIABLE(HENCKY_STRAIN_TENSOR);

    KRATOS_REGISTER_VARIABLE(VON_MISES_STRESS)

    KRATOS_REGISTER_VARIABLE(REFERENCE_DEFORMATION_GRADIENT);
    KRATOS_REGISTER_VARIABLE(REFERENCE_DEFORMATION_GRADIENT_DETERMINANT);

    // Rayleigh variables
    KRATOS_REGISTER_VARIABLE(RAYLEIGH_ALPHA)
    KRATOS_REGISTER_VARIABLE(RAYLEIGH_BETA)
    KRATOS_REGISTER_VARIABLE(SYSTEM_DAMPING_RATIO)

    // Nodal load variables
    KRATOS_REGISTER_3D_VARIABLE_WITH_COMPONENTS(POINT_LOAD)
    KRATOS_REGISTER_3D_VARIABLE_WITH_COMPONENTS(LINE_LOAD)
    KRATOS_REGISTER_3D_VARIABLE_WITH_COMPONENTS(SURFACE_LOAD)

    // Condition load variables
    KRATOS_REGISTER_VARIABLE(POINT_LOADS_VECTOR)
    KRATOS_REGISTER_VARIABLE(LINE_LOADS_VECTOR)
    KRATOS_REGISTER_VARIABLE(SURFACE_LOADS_VECTOR)
    KRATOS_REGISTER_VARIABLE(POSITIVE_FACE_PRESSURES_VECTOR)
    KRATOS_REGISTER_VARIABLE(NEGATIVE_FACE_PRESSURES_VECTOR)

    // Response function variables
    KRATOS_REGISTER_VARIABLE(RESPONSE_VALUE)
    // Adjoint variables
    KRATOS_REGISTER_3D_VARIABLE_WITH_COMPONENTS(ADJOINT_DISPLACEMENT)
    KRATOS_REGISTER_3D_VARIABLE_WITH_COMPONENTS(ADJOINT_ROTATION)
    KRATOS_REGISTER_VARIABLE(DISTURBANCE_MEASURE)

    // Variables for output of sensitivities
    KRATOS_REGISTER_VARIABLE( CROSS_AREA_SENSITIVITY );
    KRATOS_REGISTER_3D_VARIABLE_WITH_COMPONENTS( POINT_LOAD_SENSITIVITY );
    KRATOS_REGISTER_VARIABLE( I22_SENSITIVITY );
    KRATOS_REGISTER_VARIABLE( I33_SENSITIVITY );
    KRATOS_REGISTER_VARIABLE( THICKNESS_SENSITIVITY );
    KRATOS_REGISTER_VARIABLE( YOUNG_MODULUS_SENSITIVITY );
    KRATOS_REGISTER_VARIABLE( AREA_EFFECTIVE_Y_SENSITIVITY );
    KRATOS_REGISTER_VARIABLE( AREA_EFFECTIVE_Z_SENSITIVITY );
    KRATOS_REGISTER_VARIABLE( IS_ADJOINT );
    KRATOS_REGISTER_3D_VARIABLE_WITH_COMPONENTS( SHAPE );

    // Variables to for computing parts of sensitivity analysis
    KRATOS_REGISTER_VARIABLE( TRACED_STRESS_TYPE );
    KRATOS_REGISTER_VARIABLE( STRESS_DISP_DERIV_ON_GP );
    KRATOS_REGISTER_VARIABLE( STRESS_DISP_DERIV_ON_NODE);
    KRATOS_REGISTER_VARIABLE( STRESS_DESIGN_DERIVATIVE_ON_GP );
    KRATOS_REGISTER_VARIABLE( STRESS_DESIGN_DERIVATIVE_ON_NODE);
    KRATOS_REGISTER_VARIABLE( STRESS_ON_GP  );
    KRATOS_REGISTER_VARIABLE( STRESS_ON_NODE  );
    KRATOS_REGISTER_VARIABLE( DESIGN_VARIABLE_NAME );
    //KRATOS_REGISTER_VARIABLE( FINITE_DIFFERENCE_INFORMATION );



    //Register the truss element
    KRATOS_REGISTER_ELEMENT("TrussElement3D2N", mTrussElement3D2N)
    KRATOS_REGISTER_ELEMENT("TrussLinearElement3D2N", mTrussLinearElement3D2N)
    KRATOS_REGISTER_ELEMENT("CableElement3D2N", mCableElement3D2N)

    // Register the beam element
    KRATOS_REGISTER_ELEMENT("CrBeamElement3D2N", mCrBeamElement3D2N)
    KRATOS_REGISTER_ELEMENT("CrLinearBeamElement3D2N", mCrLinearBeamElement3D2N)
    KRATOS_REGISTER_ELEMENT("CrBeamElement2D2N", mCrBeamElement2D2N)
    KRATOS_REGISTER_ELEMENT("CrLinearBeamElement2D2N", mCrLinearBeamElement2D2N)

    //Register the shells elements
    KRATOS_REGISTER_ELEMENT("IsotropicShellElement3D3N", mIsotropicShellElement3D3N)
    KRATOS_REGISTER_ELEMENT("ShellThickElement3D4N", mShellThickElement3D4N)
    KRATOS_REGISTER_ELEMENT("ShellThickElementCorotational3D4N", mShellThickCorotationalElement3D4N)
    KRATOS_REGISTER_ELEMENT("ShellThinElementCorotational3D4N", mShellThinCorotationalElement3D4N)
    KRATOS_REGISTER_ELEMENT("ShellThinElement3D3N", mShellThinElement3D3N)
    KRATOS_REGISTER_ELEMENT("ShellThickElementCorotational3D3N", mShellThickCorotationalElement3D3N)
    KRATOS_REGISTER_ELEMENT("ShellThinElementCorotational3D3N", mShellThinCorotationalElement3D3N)

    // Register the membrane element
    KRATOS_REGISTER_ELEMENT("PreStressMembraneElement3D3N", mPreStressMembraneElement3D3N)
    KRATOS_REGISTER_ELEMENT("PreStressMembraneElement3D4N", mPreStressMembraneElement3D4N)

    // Register the SPRISM element
    KRATOS_REGISTER_ELEMENT("SolidShellElementSprism3D6N", mSolidShellElementSprism3D6N);

    // Register the nodal concentrated element
    KRATOS_REGISTER_ELEMENT("NodalConcentratedElement2D1N", mNodalConcentratedElement2D1N);
    KRATOS_REGISTER_ELEMENT("NodalConcentratedDampedElement2D1N", mNodalConcentratedDampedElement2D1N);
    KRATOS_REGISTER_ELEMENT("NodalConcentratedElement3D1N", mNodalConcentratedElement3D1N);
    KRATOS_REGISTER_ELEMENT("NodalConcentratedDampedElement3D1N", mNodalConcentratedDampedElement3D1N);

    // SOLID ELEMENTS
    // Small displacement elements
    KRATOS_REGISTER_ELEMENT("SmallDisplacementElement2D3N", mSmallDisplacement2D3N)
    KRATOS_REGISTER_ELEMENT("SmallDisplacementElement2D4N", mSmallDisplacement2D4N)
    KRATOS_REGISTER_ELEMENT("SmallDisplacementElement2D6N", mSmallDisplacement2D6N)
    KRATOS_REGISTER_ELEMENT("SmallDisplacementElement2D8N", mSmallDisplacement2D8N)
    KRATOS_REGISTER_ELEMENT("SmallDisplacementElement2D9N", mSmallDisplacement2D9N)
    KRATOS_REGISTER_ELEMENT("SmallDisplacementElement3D4N", mSmallDisplacement3D4N)
    KRATOS_REGISTER_ELEMENT("SmallDisplacementElement3D6N", mSmallDisplacement3D6N)
    KRATOS_REGISTER_ELEMENT("SmallDisplacementElement3D8N", mSmallDisplacement3D8N)
    KRATOS_REGISTER_ELEMENT("SmallDisplacementElement3D10N", mSmallDisplacement3D10N)
    KRATOS_REGISTER_ELEMENT("SmallDisplacementElement3D15N", mSmallDisplacement3D15N)
    KRATOS_REGISTER_ELEMENT("SmallDisplacementElement3D20N", mSmallDisplacement3D20N)
    KRATOS_REGISTER_ELEMENT("SmallDisplacementElement3D27N", mSmallDisplacement3D27N)

    KRATOS_REGISTER_ELEMENT("SmallDisplacementBbarElement2D4N", mSmallDisplacementBbar2D4N)
    KRATOS_REGISTER_ELEMENT("SmallDisplacementBbarElement3D8N", mSmallDisplacementBbar3D8N)

    KRATOS_REGISTER_ELEMENT("AxisymSmallDisplacementElement2D3N", mAxisymSmallDisplacement2D3N)
    KRATOS_REGISTER_ELEMENT("AxisymSmallDisplacementElement2D4N", mAxisymSmallDisplacement2D4N)
    KRATOS_REGISTER_ELEMENT("AxisymSmallDisplacementElement2D6N", mAxisymSmallDisplacement2D6N)
    KRATOS_REGISTER_ELEMENT("AxisymSmallDisplacementElement2D8N", mAxisymSmallDisplacement2D8N)
    KRATOS_REGISTER_ELEMENT("AxisymSmallDisplacementElement2D9N", mAxisymSmallDisplacement2D9N)

    // Total lagrangian elements
    KRATOS_REGISTER_ELEMENT("TotalLagrangianElement2D3N", mTotalLagrangian2D3N)
    KRATOS_REGISTER_ELEMENT("TotalLagrangianElement2D4N", mTotalLagrangian2D4N)
    KRATOS_REGISTER_ELEMENT("TotalLagrangianElement2D6N", mTotalLagrangian2D6N)
    KRATOS_REGISTER_ELEMENT("TotalLagrangianElement2D8N", mTotalLagrangian2D8N)
    KRATOS_REGISTER_ELEMENT("TotalLagrangianElement2D9N", mTotalLagrangian2D9N)
    KRATOS_REGISTER_ELEMENT("TotalLagrangianElement3D4N", mTotalLagrangian3D4N)
    KRATOS_REGISTER_ELEMENT("TotalLagrangianElement3D6N", mTotalLagrangian3D6N)
    KRATOS_REGISTER_ELEMENT("TotalLagrangianElement3D8N", mTotalLagrangian3D8N)
    KRATOS_REGISTER_ELEMENT("TotalLagrangianElement3D10N", mTotalLagrangian3D10N)
    KRATOS_REGISTER_ELEMENT("TotalLagrangianElement3D15N", mTotalLagrangian3D15N)
    KRATOS_REGISTER_ELEMENT("TotalLagrangianElement3D20N", mTotalLagrangian3D20N)
    KRATOS_REGISTER_ELEMENT("TotalLagrangianElement3D27N", mTotalLagrangian3D27N)

    KRATOS_REGISTER_ELEMENT("AxisymTotalLagrangianElement2D3N", mAxisymTotalLagrangian2D3N)
    KRATOS_REGISTER_ELEMENT("AxisymTotalLagrangianElement2D4N", mAxisymTotalLagrangian2D4N)
    KRATOS_REGISTER_ELEMENT("AxisymTotalLagrangianElement2D6N", mAxisymTotalLagrangian2D6N)
    KRATOS_REGISTER_ELEMENT("AxisymTotalLagrangianElement2D8N", mAxisymTotalLagrangian2D8N)
    KRATOS_REGISTER_ELEMENT("AxisymTotalLagrangianElement2D9N", mAxisymTotalLagrangian2D9N)

    // Updated lagrangian elements
    KRATOS_REGISTER_ELEMENT("UpdatedLagrangianElement2D3N", mUpdatedLagrangian2D3N)
    KRATOS_REGISTER_ELEMENT("UpdatedLagrangianElement2D4N", mUpdatedLagrangian2D4N)
    KRATOS_REGISTER_ELEMENT("UpdatedLagrangianElement2D6N", mUpdatedLagrangian2D6N)
    KRATOS_REGISTER_ELEMENT("UpdatedLagrangianElement2D8N", mUpdatedLagrangian2D8N)
    KRATOS_REGISTER_ELEMENT("UpdatedLagrangianElement2D9N", mUpdatedLagrangian2D9N)
    KRATOS_REGISTER_ELEMENT("UpdatedLagrangianElement3D4N", mUpdatedLagrangian3D4N)
    KRATOS_REGISTER_ELEMENT("UpdatedLagrangianElement3D6N", mUpdatedLagrangian3D6N)
    KRATOS_REGISTER_ELEMENT("UpdatedLagrangianElement3D8N", mUpdatedLagrangian3D8N)
    KRATOS_REGISTER_ELEMENT("UpdatedLagrangianElement3D10N", mUpdatedLagrangian3D10N)
    KRATOS_REGISTER_ELEMENT("UpdatedLagrangianElement3D15N", mUpdatedLagrangian3D15N)
    KRATOS_REGISTER_ELEMENT("UpdatedLagrangianElement3D20N", mUpdatedLagrangian3D20N)
    KRATOS_REGISTER_ELEMENT("UpdatedLagrangianElement3D27N", mUpdatedLagrangian3D27N)

    KRATOS_REGISTER_ELEMENT("AxisymUpdatedLagrangianElement2D3N", mAxisymUpdatedLagrangian2D3N)
    KRATOS_REGISTER_ELEMENT("AxisymUpdatedLagrangianElement2D4N", mAxisymUpdatedLagrangian2D4N)
    KRATOS_REGISTER_ELEMENT("AxisymUpdatedLagrangianElement2D6N", mAxisymUpdatedLagrangian2D6N)
    KRATOS_REGISTER_ELEMENT("AxisymUpdatedLagrangianElement2D8N", mAxisymUpdatedLagrangian2D8N)
    KRATOS_REGISTER_ELEMENT("AxisymUpdatedLagrangianElement2D9N", mAxisymUpdatedLagrangian2D9N)

    // Register the spring damper element
    KRATOS_REGISTER_ELEMENT("SpringDamperElement3D2N", mSpringDamperElement3D2N);

    //Register the adjoint elements
    KRATOS_REGISTER_ELEMENT( "ShellThinAdjointElement3D3N", mShellThinAdjointElement3D3N )
    KRATOS_REGISTER_ELEMENT( "CrLinearBeamAdjointElement3D2N", mCrLinearBeamAdjointElement3D2N )

    // Register the conditions
    // Point loads
    KRATOS_REGISTER_CONDITION("PointLoadCondition2D1N", mPointLoadCondition2D1N)
    KRATOS_REGISTER_CONDITION("PointLoadCondition3D1N", mPointLoadCondition3D1N)
    KRATOS_REGISTER_CONDITION("PointContactCondition2D1N", mPointContactCondition2D1N)
    KRATOS_REGISTER_CONDITION("PointContactCondition3D1N", mPointContactCondition3D1N)

    KRATOS_REGISTER_CONDITION("AxisymPointLoadCondition2D1N", mAxisymPointLoadCondition2D1N)

    // Line loads
    KRATOS_REGISTER_CONDITION("LineLoadCondition2D2N", mLineLoadCondition2D2N)
    KRATOS_REGISTER_CONDITION("LineLoadCondition2D3N", mLineLoadCondition2D3N)

    KRATOS_REGISTER_CONDITION("AxisymLineLoadCondition2D2N", mAxisymLineLoadCondition2D2N)
    KRATOS_REGISTER_CONDITION("AxisymLineLoadCondition2D3N", mAxisymLineLoadCondition2D3N)

    // Surface loads
    KRATOS_REGISTER_CONDITION("SurfaceLoadCondition3D3N", mSurfaceLoadCondition3D3N)
    KRATOS_REGISTER_CONDITION("SurfaceLoadCondition3D4N", mSurfaceLoadCondition3D4N)
    KRATOS_REGISTER_CONDITION("SurfaceLoadCondition3D6N", mSurfaceLoadCondition3D6N)
    KRATOS_REGISTER_CONDITION("SurfaceLoadCondition3D8N", mSurfaceLoadCondition3D8N)
    KRATOS_REGISTER_CONDITION("SurfaceLoadCondition3D9N", mSurfaceLoadCondition3D9N)

    // Point moment
    KRATOS_REGISTER_CONDITION("PointMomentCondition3D1N", mPointMomentCondition3D1N);

    // Adjoint conditions
    KRATOS_REGISTER_CONDITION( "PointLoadAdjointCondition2D1N", mPointLoadAdjointCondition2D1N )
    KRATOS_REGISTER_CONDITION( "PointLoadAdjointCondition3D1N", mPointLoadAdjointCondition3D1N )

    // For MPC implementations
    KRATOS_REGISTER_VARIABLE(MPC_DATA_CONTAINER)

    KRATOS_REGISTER_VARIABLE(INELASTIC_FLAG)
    KRATOS_REGISTER_VARIABLE(INFINITY_YIELD_STRESS)
    // Register linear elastics laws
    KRATOS_REGISTER_CONSTITUTIVE_LAW("TrussConstitutiveLaw", mTrussConstitutiveLaw);
    KRATOS_REGISTER_CONSTITUTIVE_LAW("TrussPlasticityConstitutiveLaw", mTrussPlasticityConstitutiveLaw);
    KRATOS_REGISTER_CONSTITUTIVE_LAW("BeamConstitutiveLaw", mBeamConstitutiveLaw);
    KRATOS_REGISTER_CONSTITUTIVE_LAW("LinearElastic3DLaw", mElasticIsotropic3D);
    KRATOS_REGISTER_CONSTITUTIVE_LAW("LinearElasticPlaneStrain2DLaw", mLinearPlaneStrain);
    KRATOS_REGISTER_CONSTITUTIVE_LAW("LinearElasticPlaneStress2DLaw", mLinearPlaneStress);
    KRATOS_REGISTER_CONSTITUTIVE_LAW("ElasticPlaneStressUncoupledShear2DLaw", mElasticIsotropicPlaneStressUncoupledShear);
    KRATOS_REGISTER_CONSTITUTIVE_LAW("LinearElasticAxisym2DLaw", mAxisymElasticIsotropic);
    KRATOS_REGISTER_CONSTITUTIVE_LAW("LinearElasticOrthotropic2DLaw", mLinearElasticOrthotropic2DLaw);
    // Register hyper elastic laws
    KRATOS_REGISTER_CONSTITUTIVE_LAW("KirchhoffSaintVenant3DLaw", mHyperElasticIsotropicKirchhoff3D);
    KRATOS_REGISTER_CONSTITUTIVE_LAW("KirchhoffSaintVenantPlaneStress2DLaw", mHyperElasticIsotropicKirchhoffPlaneStress2D);
    KRATOS_REGISTER_CONSTITUTIVE_LAW("KirchhoffSaintVenantPlaneStrain2DLaw", mHyperElasticIsotropicKirchhoffPlaneStrain2D);
    KRATOS_REGISTER_CONSTITUTIVE_LAW("HyperElastic3DLaw", mHyperElasticIsotropicNeoHookean3D);
    KRATOS_REGISTER_CONSTITUTIVE_LAW("HyperElasticPlaneStrain2DLaw", mHyperElasticIsotropicNeoHookeanPlaneStrain2D);
    KRATOS_REGISTER_CONSTITUTIVE_LAW("LinearJ2PlasticityPlaneStrain2DLaw", mLinearJ2PlasticityPlaneStrain2D);
    KRATOS_REGISTER_CONSTITUTIVE_LAW("LinearJ2Plasticity3DLaw", mLinearJ2Plasticity3D);
    KRATOS_REGISTER_CONSTITUTIVE_LAW("LinearIsotropicDamage3DLaw", mLinearIsotropicDamage3D);
}

}  // namespace Kratos.
