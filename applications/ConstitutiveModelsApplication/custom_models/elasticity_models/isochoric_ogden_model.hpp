//
//   Project Name:        KratosConstitutiveModelsApplication $
//   Created by:          $Author:                JMCarbonell $
//   Last modified by:    $Co-Author:                         $
//   Date:                $Date:                   April 2017 $
//   Revision:            $Revision:                      0.0 $
//
//

#if !defined(KRATOS_ISOCHORIC_OGDEN_MODEL_H_INCLUDED )
#define  KRATOS_ISOCHORIC_OGDEN_MODEL_H_INCLUDED

// System includes

// External includes

// Project includes
#include "custom_models/elasticity_models/ogden_model.hpp"

namespace Kratos
{
  ///@addtogroup ConstitutiveModelsApplication
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

  /// Short class definition.
  /** Detail class definition.
   */
  class KRATOS_API(CONSTITUTIVE_MODELS_APPLICATION) IsochoricOgdenModel : public OgdenModel
  {
  public:

    ///@name Type Definitions
    ///@{

    /// Pointer definition of IsochoricOgdenModel
    KRATOS_CLASS_POINTER_DEFINITION( IsochoricOgdenModel );

    ///@}
    ///@name Life Cycle
    ///@{

    /// Default constructor.
    IsochoricOgdenModel() : OgdenModel() {}
    
    /// Copy constructor.
    IsochoricOgdenModel(IsochoricOgdenModel const& rOther) : OgdenModel(rOther) {}

    /// Assignment operator.
    IsochoricOgdenModel& operator=(IsochoricOgdenModel const& rOther)
    {
	OgdenModel::operator=(rOther);
	return *this;
    }

    /// Clone.
    virtual ConstitutiveModel::Pointer Clone() const override
    {
      return ( IsochoricOgdenModel::Pointer(new IsochoricOgdenModel(*this)) );      
    }
 
    /// Destructor.
    virtual ~IsochoricOgdenModel() {}


    ///@}
    ///@name Operators
    ///@{


    ///@}
    ///@name Operations
    ///@{
  

    virtual void CalculateStrainEnergy(ModelDataType& rValues, double& rDensityFunction) override
    {
      KRATOS_TRY

      HyperElasticDataType Variables;
      this->CalculateStrainData(rValues, Variables);

      rDensityFunction = 0;
      this->CalculateAndAddIsochoricStrainEnergy( Variables, rDensityFunction );
      this->CalculateAndAddVolumetricStrainEnergy( Variables, rDensityFunction );

	
      KRATOS_CATCH(" ")
    }


    virtual void CalculateStressTensor(ModelDataType& rValues, MatrixType& rStressMatrix) override
    {
      KRATOS_TRY

      HyperElasticDataType Variables;
      this->CalculateStrainData(rValues,Variables);

      this->CalculateAndAddIsochoricStressTensor(Variables, rStressMatrix);
      
      rValues.StressMatrix = rStressMatrix; //store isochoric stress matrix as StressMatrix

      this->CalculateAndAddVolumetricStressTensor(Variables, rStressMatrix);

      Variables.State().Set(ConstitutiveModelData::STRESS_COMPUTED);
      
      KRATOS_CATCH(" ")
    }

    
    virtual void CalculateConstitutiveTensor(ModelDataType& rValues, Matrix& rConstitutiveMatrix) override
    {
	KRATOS_TRY

        //Initialize ConstitutiveMatrix  
	HyperElasticDataType Variables;
	this->CalculateStrainData(rValues,Variables);
    
	//Calculate Constitutive Matrix
	this->CalculateAndAddConstitutiveTensor(Variables,rConstitutiveMatrix);   
    	
	KRATOS_CATCH(" ")
    }

    
    virtual void CalculateStressAndConstitutiveTensors(ModelDataType& rValues, MatrixType& rStressMatrix, Matrix& rConstitutiveMatrix) override
    {
      KRATOS_TRY
     
      HyperElasticDataType Variables;
      this->CalculateStrainData(rValues,Variables);

      //Calculate Stress Matrix
      this->CalculateAndAddIsochoricStressTensor(Variables, rStressMatrix);
      
      rValues.StressMatrix = rStressMatrix; //store isochoric stress matrix as StressMatrix

      this->CalculateAndAddVolumetricStressTensor(Variables, rStressMatrix);

      //Calculate Constitutive Matrix
      this->CalculateAndAddConstitutiveTensor(Variables,rConstitutiveMatrix);
      
      KRATOS_CATCH(" ")
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
    virtual std::string Info() const override
    {
        std::stringstream buffer;
        buffer << "IsochoricOgdenModel";
        return buffer.str();
    }

    /// Print information about this object.
    virtual void PrintInfo(std::ostream& rOStream) const override
    {
        rOStream << "IsochoricOgdenModel";
    }

    /// Print object's data.
    virtual void PrintData(std::ostream& rOStream) const override
    {
      rOStream << "IsochoricOgdenModel Data";
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

    virtual void CalculateStrainData(ModelDataType& rValues, HyperElasticDataType& rVariables) override
    {
	KRATOS_TRY

	OgdenModel::CalculateStrainData(rValues,rVariables);
	    
	//Isochoric eigenvalues
	for(unsigned int i=0; i<3; i++)
	{
	    rVariables.Strain.Eigen.Values[i] = rVariables.Strain.Eigen.Values[i] / std::pow(rVariables.Strain.Invariants.J, 1.0/3.0);
	}

	//Calculate Invariants
	this->CalculateInvariants(rVariables);

	//Algorithmic moduli factors
	this->CalculateScalingFactors(rVariables);

	KRATOS_CATCH(" ")
    }

    
    virtual void CalculateInvariants(HyperElasticDataType& rVariables) override
    {
	KRATOS_TRY
      
        //invariants
	rVariables.Strain.Invariants.I1 = rVariables.Strain.Eigen.Values[0] * rVariables.Strain.Eigen.Values[0] +
	                                  rVariables.Strain.Eigen.Values[1] * rVariables.Strain.Eigen.Values[1] +
	                                  rVariables.Strain.Eigen.Values[2] * rVariables.Strain.Eigen.Values[2];
	
	rVariables.Strain.Invariants.I2 = rVariables.Strain.Eigen.Values[1] * rVariables.Strain.Eigen.Values[1] *
	                                  rVariables.Strain.Eigen.Values[2] * rVariables.Strain.Eigen.Values[2] +
 	                                  rVariables.Strain.Eigen.Values[2] * rVariables.Strain.Eigen.Values[0] *
	                                  rVariables.Strain.Eigen.Values[2] * rVariables.Strain.Eigen.Values[0] +
   	                                  rVariables.Strain.Eigen.Values[0] * rVariables.Strain.Eigen.Values[1] *
	                                  rVariables.Strain.Eigen.Values[0] * rVariables.Strain.Eigen.Values[1];
	
	rVariables.Strain.Invariants.I3 = rVariables.Strain.Eigen.Values[0] * rVariables.Strain.Eigen.Values[0] *
	                                  rVariables.Strain.Eigen.Values[1] * rVariables.Strain.Eigen.Values[1] *
  	                                  rVariables.Strain.Eigen.Values[2] * rVariables.Strain.Eigen.Values[2];

    
	//jacobian
	rVariables.Strain.Invariants.J    = rVariables.GetModelData().GetTotalDeformationDet();
	rVariables.Strain.Invariants.J_13 = pow(rVariables.Strain.Invariants.J,(-1.0/3.0));


	//rVariables.Strain.Invariants.I3 = rVariables.Strain.Invariants.J * rVariables.Strain.Invariants.J; //for volumetric consistency
	
	//std::cout<<" Strain.Invariants [I1:"<<rVariables.Strain.Invariants.I1<<" I2:"<<rVariables.Strain.Invariants.I2<<" I3:"<<rVariables.Strain.Invariants.I3<<"] J:"<<rVariables.Strain.Invariants.J<<std::endl;
	KRATOS_CATCH(" ")
    }

    

    virtual void CalculateAndAddIsochoricStressTensor(HyperElasticDataType& rVariables, MatrixType& rStressMatrix) override
    {
      KRATOS_TRY

      const ModelDataType&  rModelData        = rVariables.GetModelData();
      const StressMeasureType& rStressMeasure = rModelData.GetStressMeasure();

      array_1d<double,3> MainStresses;
      this->CalculateMainStresses(rVariables,MainStresses);
    
      if( rStressMeasure == ConstitutiveModelData::StressMeasure_PK2 ){ //Strain.Matrix = RightCauchyGreen (C)
	  array_1d<double,3> EigenVector;

	  for(unsigned int i=0; i<3; i++)
	  {
	      noalias(EigenVector) = matrix_row<const MatrixType>(rVariables.Strain.Eigen.Vectors,i);
	      EigenVector /= rVariables.Strain.Eigen.Values[i];
	      noalias(rStressMatrix) += MainStresses[i] * outer_prod(EigenVector,EigenVector);
	  }

      }
      else if( rStressMeasure == ConstitutiveModelData::StressMeasure_Kirchhoff ){ //Strain.Matrix = LeftCauchyGreen (b)
	  
	  array_1d<double,3> EigenVector;
	  for(unsigned int i=0; i<3; i++)
	  {
	      noalias(EigenVector) = matrix_row<const MatrixType>(rVariables.Strain.Eigen.Vectors,i);
	      noalias(rStressMatrix) += MainStresses[i] * outer_prod(EigenVector,EigenVector);
	  }

      }

      
      KRATOS_CATCH(" ")
    }


    virtual void CalculateAndAddVolumetricStressTensor(HyperElasticDataType& rVariables, MatrixType& rStressMatrix) override
    {
      KRATOS_TRY

      const ModelDataType&  rModelData        = rVariables.GetModelData();
      const StressMeasureType& rStressMeasure = rModelData.GetStressMeasure();
	
      MatrixType StressMatrix;	  
      if( rStressMeasure == ConstitutiveModelData::StressMeasure_PK2 ){ //Variables.Strain.Matrix = RightCauchyGreen (C)
	StressMatrix  = GetJRightCauchyGreenDerivative(rVariables.Strain,StressMatrix);
	StressMatrix *= rVariables.Factors.Alpha4;
	
	StressMatrix *= 2.0;
	
	noalias(rStressMatrix) += StressMatrix;
      }
      else if( rStressMeasure == ConstitutiveModelData::StressMeasure_Kirchhoff ){ //Variables.Strain.Matrix = LeftCauchyGreen (b)
		    
	StressMatrix  = GetJLeftCauchyGreenDerivative(rVariables.Strain,StressMatrix);
	StressMatrix *= rVariables.Factors.Alpha4;
	
	StressMatrix *= 2.0; 
	
	noalias(rStressMatrix) += StressMatrix;
      }      	
      
      KRATOS_CATCH(" ")
    }


    //************************************************************************************
    //************************************************************************************
  
    virtual void CalculateMainStresses(HyperElasticDataType& rVariables, array_1d<double,3>& rMainStresses) override
    {
	KRATOS_TRY
	    
 
	const MaterialDataType& rMaterial = rVariables.GetMaterialParameters();
	const std::vector<double>& rModelParameters = rMaterial.GetModelParameters(); //nu values, lambda values

	unsigned int size = (rModelParameters.size()/2.0);
	double athird = 1.0/3.0;
	
	for(unsigned int i=0; i<3; i++)
	{
	    for(unsigned int p=0; p<size; p++)
	    {
		const double& mu_p = rModelParameters[p];
		const double& alpha_p = rModelParameters[p+size];
		rMainStresses[i] += (mu_p) * ( std::pow(rVariables.Strain.Eigen.Values[i],alpha_p) - athird * ( std::pow(rVariables.Strain.Eigen.Values[0],alpha_p) + std::pow(rVariables.Strain.Eigen.Values[1],alpha_p) + std::pow(rVariables.Strain.Eigen.Values[2],alpha_p) ) );
	    }	    
	    
	}
	    
	KRATOS_CATCH(" ")
    }
    

    //************************************************************************************
    //************************************************************************************
  
    virtual void CalculateMainStressDerivatives(HyperElasticDataType& rVariables, MatrixType& rStressDerivatives) override
    {
	KRATOS_TRY
	    
	//Isochoric eigenvalues

	const MaterialDataType& rMaterial = rVariables.GetMaterialParameters();
	const std::vector<double>& rModelParameters = rMaterial.GetModelParameters(); //nu values, lambda values

	unsigned int size = (rModelParameters.size()/2.0);
	double athird = 1.0/3.0;
	
	for(unsigned int i=0; i<3; i++)
	{
	    for(unsigned int j=0; j<3; j++)
	    {

		for(unsigned int p=0; p<size; p++)
		{
		    const double& mu_p = rModelParameters[p];
		    const double& alpha_p = rModelParameters[p+size];

		    rStressDerivatives(i,j) += (mu_p) * ( athird * (std::pow(rVariables.Strain.Eigen.Values[i],alpha_p) + athird * ( std::pow(rVariables.Strain.Eigen.Values[0],alpha_p) + std::pow(rVariables.Strain.Eigen.Values[1],alpha_p) + std::pow(rVariables.Strain.Eigen.Values[2],alpha_p) ) ) );

		    if( i != j ){
			rStressDerivatives(i,j) -= (mu_p) * ( athird * ( 2.0 * std::pow(rVariables.Strain.Eigen.Values[i],alpha_p) + std::pow(rVariables.Strain.Eigen.Values[j],alpha_p) ) );
		    }
		}
	    }

	}
	    
    
	KRATOS_CATCH(" ")
    }

	    
    //************************************************************************************
    //************************************************************************************
  
    virtual void CalculateAndAddConstitutiveTensor(HyperElasticDataType& rVariables, Matrix& rConstitutiveMatrix) override
    {
	KRATOS_TRY
              
	//Calculate Ogden ConstitutiveMatrix
	const ModelDataType&  rModelData        = rVariables.GetModelData();
	const SizeType&       rVoigtSize        = rModelData.GetVoigtSize();      
	const VoigtIndexType& rIndexVoigtTensor = rModelData.GetVoigtIndexTensor();

	//Calculate Stress main streches derivatives
	MatrixType StressDerivatives;
	noalias(StressDerivatives)=ZeroMatrix(3,3);
	this->CalculateMainStressDerivatives(rVariables, StressDerivatives);
        
	array_1d<double,3> StressEigenValues;
	noalias(StressEigenValues)=ZeroVector(3);
	this->CalculateMainStresses(rVariables, StressEigenValues);	

	//Calculate constitutive components 
	for(SizeType i=0; i<rVoigtSize; i++)
	{
	    for(SizeType j=0; j<rVoigtSize; j++)
		{
		    rConstitutiveMatrix(i,j) = this->AddIsochoricConstitutiveComponent(rVariables,rConstitutiveMatrix(i,j),
										       StressDerivatives,StressEigenValues,
										       rIndexVoigtTensor[i][0],rIndexVoigtTensor[i][1],
										       rIndexVoigtTensor[j][0],rIndexVoigtTensor[j][1]);
		    //std::cout<<" Cij "<<rConstitutiveMatrix(i,j)<<" "<<i<<" "<<j<<std::endl;
		    rConstitutiveMatrix(i,j) = this->AddVolumetricConstitutiveComponent(rVariables,rConstitutiveMatrix(i,j),
											rIndexVoigtTensor[i][0],rIndexVoigtTensor[i][1],
											rIndexVoigtTensor[j][0],rIndexVoigtTensor[j][1]);
		    //std::cout<<" Cij "<<rConstitutiveMatrix(i,j)<<" "<<i<<" "<<j<<std::endl;
		}
	    
	}
	
	rVariables.State().Set(ConstitutiveModelData::CONSTITUTIVE_MATRIX_COMPUTED,true);
        
    
	KRATOS_CATCH(" ")
    }



    
    virtual double& AddIsochoricConstitutiveComponent(HyperElasticDataType& rVariables, double &rCabcd,
						      const MatrixType& rStressDerivatives, const array_1d<double,3>& rStressEigenValues,
						      const unsigned int& a, const unsigned int& b,
						      const unsigned int& c, const unsigned int& d) //do not override
    { 
      KRATOS_TRY
     
      const ModelDataType& rModelData         = rVariables.GetModelData();
      const StressMeasureType& rStressMeasure = rModelData.GetStressMeasure();
	
      double Dabcd = 0;
      double Cabcd = 0;
      
      array_1d<double,3> EigenVectorA;
      array_1d<double,3> EigenVectorB;    
      
      if( rStressMeasure == ConstitutiveModelData::StressMeasure_PK2 ){ //Variables.Strain.Matrix = RightCauchyGreen (C)
	  for(unsigned int i=0; i<3; i++)
	  {
	      noalias(EigenVectorA) = matrix_row<const MatrixType>(rVariables.Strain.Eigen.Vectors,i);
	      EigenVectorA /= rVariables.Strain.Eigen.Values[i];
	      for(unsigned int j=0; j<3; j++)
	      {
		  
		  noalias(EigenVectorB) = matrix_row<const MatrixType>(rVariables.Strain.Eigen.Vectors,j);
		  EigenVectorB /= rVariables.Strain.Eigen.Values[j];
								
		  Dabcd = ConstitutiveModelUtilities::CalculateFourthOrderTensorProduct(EigenVectorA,EigenVectorB,Dabcd,a,b,c,d);

		  Cabcd += rStressDerivatives(i,j) * Dabcd;
	      }

	      Dabcd  = GetEigenProductRightCauchyGreenDerivative(rVariables,i,Dabcd,a,b,c,d);
	      Cabcd += 2.0 * rStressEigenValues[i] * Dabcd;
	      //std::cout<<" Cabcd "<<Cabcd<<" Dabcd "<<Dabcd<<" "<<a<<" "<<b<<" "<<c<<" "<<d<<std::endl;
	  }
	  
      }
      else if( rStressMeasure == ConstitutiveModelData::StressMeasure_Kirchhoff ){ //Variables.Strain.M

	  for(unsigned int i=0; i<3; i++)
	  {
	      noalias(EigenVectorA) = matrix_row<const MatrixType>(rVariables.Strain.Eigen.Vectors,i);
	      for(unsigned int j=0; j<3; j++)
	      {
		  
		  noalias(EigenVectorB) = matrix_row<const MatrixType>(rVariables.Strain.Eigen.Vectors,j);
								
		  Dabcd = ConstitutiveModelUtilities::CalculateFourthOrderTensorProduct(EigenVectorA,EigenVectorB,Dabcd,a,b,c,d);

		  Cabcd += rStressDerivatives(i,j) * Dabcd;
	      }

	      Dabcd  = GetEigenProductLeftCauchyGreenDerivative(rVariables,i,Dabcd,a,b,c,d);
	      Cabcd += 4.0 * rStressEigenValues[i] * Dabcd;
	      //std::cout<<" Cabcd "<<Cabcd<<" Dabcd "<<Dabcd<<" "<<a<<" "<<b<<" "<<c<<" "<<d<<std::endl;
	  }

      }

      rCabcd += Cabcd;
      
      return rCabcd;
	
      KRATOS_CATCH(" ")
    }



    virtual double& GetEigenProductRightCauchyGreenDerivative(HyperElasticDataType& rVariables, const unsigned int& i, double &rCabcd,
							      const unsigned int& a, const unsigned int& b,
							      const unsigned int& c, const unsigned int& d)
    {
      KRATOS_TRY

      const double& lambda = rVariables.Strain.Eigen.Values[i];	 

      double D = 2.0 * lambda*lambda*lambda*lambda - rVariables.Strain.Invariants.I1 * lambda*lambda + rVariables.Strain.Invariants.I3 / (lambda*lambda);

      double dD = 8.0 * lambda*lambda*lambda - 2.0 * rVariables.Strain.Invariants.I1 * lambda - 2.0 * rVariables.Strain.Invariants.I3 / (lambda*lambda*lambda);

      double Dabcd = 0;

      array_1d<double,3> EigenVector;
      noalias(EigenVector) = matrix_row<const MatrixType>(rVariables.Strain.Eigen.Vectors,i);

      Dabcd = ConstitutiveModelUtilities::CalculateFourthOrderUnitTensor(this->msIdentityMatrix,Dabcd,a,b,c,d);
      rCabcd += Dabcd;
      Dabcd = ConstitutiveModelUtilities::CalculateFourthOrderTensorProduct(this->msIdentityMatrix,this->msIdentityMatrix,Dabcd,a,b,c,d);	  
      rCabcd += Dabcd;
      Dabcd = ConstitutiveModelUtilities::CalculateFourthOrderTensorProduct(rVariables.Strain.InverseMatrix,rVariables.Strain.InverseMatrix,Dabcd,a,b,c,d);
      rCabcd += Dabcd * rVariables.Strain.Invariants.I3 / (lambda*lambda);
      Dabcd = ConstitutiveModelUtilities::CalculateFourthOrderTensor(rVariables.Strain.InverseMatrix,Dabcd,a,b,c,d);
      rCabcd -= Dabcd * rVariables.Strain.Invariants.I3 / (lambda*lambda);
      Dabcd = ConstitutiveModelUtilities::CalculateFourthOrderTensorProduct(this->msIdentityMatrix,EigenVector,Dabcd,a,b,c,d);	  
      rCabcd += Dabcd;
      Dabcd = ConstitutiveModelUtilities::CalculateFourthOrderTensorProduct(EigenVector,this->msIdentityMatrix,Dabcd,a,b,c,d);	  
      rCabcd += Dabcd;
      Dabcd = ConstitutiveModelUtilities::CalculateFourthOrderTensorProduct(EigenVector,EigenVector,Dabcd,a,b,c,d);
      rCabcd -= 0.5 * dD * Dabcd / (lambda*lambda*lambda);
      Dabcd = ConstitutiveModelUtilities::CalculateFourthOrderTensorProduct(rVariables.Strain.InverseMatrix,EigenVector,Dabcd,a,b,c,d);
      rCabcd -= rVariables.Strain.Invariants.I3 * Dabcd / (lambda*lambda*lambda*lambda);
      Dabcd = ConstitutiveModelUtilities::CalculateFourthOrderTensorProduct(EigenVector,rVariables.Strain.InverseMatrix,Dabcd,a,b,c,d);
      rCabcd -= rVariables.Strain.Invariants.I3 * Dabcd / (lambda*lambda*lambda*lambda);

      rCabcd *= D;

      //std::cout<<" Cabcd "<<rCabcd<<" "<<a<<" "<<b<<" "<<c<<" "<<d<<std::endl;
      
      return rCabcd;
	    
      KRATOS_CATCH(" ")
	
    }

    virtual double& GetEigenProductLeftCauchyGreenDerivative(HyperElasticDataType& rVariables, const unsigned int& i, double &rCabcd,
							      const unsigned int& a, const unsigned int& b,
							      const unsigned int& c, const unsigned int& d)
    {
      KRATOS_TRY

      const double& lambda = rVariables.Strain.Eigen.Values[i];	 

      double D = 2.0 * lambda*lambda*lambda*lambda - rVariables.Strain.Invariants.I1 * lambda*lambda + rVariables.Strain.Invariants.I3 / (lambda*lambda);

      double dD = 8.0 * lambda*lambda*lambda - 2.0 * rVariables.Strain.Invariants.I1 * lambda - 2.0 * rVariables.Strain.Invariants.I3 / (lambda*lambda*lambda);

      double Dabcd = 0;
      
      array_1d<double,3> EigenVector;
      noalias(EigenVector) = matrix_row<const MatrixType>(rVariables.Strain.Eigen.Vectors,i);

      Dabcd = ConstitutiveModelUtilities::CalculateFourthOrderTensor(rVariables.Strain.Matrix,Dabcd,a,b,c,d);
      rCabcd += Dabcd;
      Dabcd = ConstitutiveModelUtilities::CalculateFourthOrderTensorProduct(rVariables.Strain.Matrix,rVariables.Strain.Matrix,Dabcd,a,b,c,d);	  
      rCabcd -= Dabcd;
      Dabcd = ConstitutiveModelUtilities::CalculateFourthOrderTensorProduct(this->msIdentityMatrix,this->msIdentityMatrix,Dabcd,a,b,c,d);
      rCabcd += Dabcd * rVariables.Strain.Invariants.I3 / (lambda*lambda);
      Dabcd = ConstitutiveModelUtilities::CalculateFourthOrderUnitTensor(this->msIdentityMatrix,Dabcd,a,b,c,d);
      rCabcd -= Dabcd * rVariables.Strain.Invariants.I3 / (lambda*lambda);      
      Dabcd = ConstitutiveModelUtilities::CalculateFourthOrderTensorProduct(rVariables.Strain.Matrix,EigenVector,Dabcd,a,b,c,d);	  
      rCabcd += (lambda*lambda) * Dabcd;
      Dabcd = ConstitutiveModelUtilities::CalculateFourthOrderTensorProduct(EigenVector,rVariables.Strain.Matrix,Dabcd,a,b,c,d);	  
      rCabcd += (lambda*lambda) * Dabcd;
      Dabcd = ConstitutiveModelUtilities::CalculateFourthOrderTensorProduct(EigenVector,EigenVector,Dabcd,a,b,c,d);
      rCabcd -= 0.5 * dD * lambda * Dabcd;
      Dabcd = ConstitutiveModelUtilities::CalculateFourthOrderTensorProduct(this->msIdentityMatrix,EigenVector,Dabcd,a,b,c,d);
      rCabcd -= rVariables.Strain.Invariants.I3 * Dabcd / (lambda*lambda);
      Dabcd = ConstitutiveModelUtilities::CalculateFourthOrderTensorProduct(EigenVector,this->msIdentityMatrix,Dabcd,a,b,c,d);
      rCabcd -= rVariables.Strain.Invariants.I3 * Dabcd / (lambda*lambda);

      rCabcd *= D;
      
      //std::cout<<" Cabcd "<<rCabcd<<" "<<a<<" "<<b<<" "<<c<<" "<<d<<std::endl;

      return rCabcd;
	    
      KRATOS_CATCH(" ")
	
    }
    
    virtual double& AddVolumetricConstitutiveComponent(HyperElasticDataType& rVariables, double &rCabcd,
						       const unsigned int& a, const unsigned int& b,
						       const unsigned int& c, const unsigned int& d) override
    {
      KRATOS_TRY

      const ModelDataType& rModelData         = rVariables.GetModelData();
      const StressMeasureType& rStressMeasure = rModelData.GetStressMeasure();
	
      double Dabcd = 0;
      double Cabcd = 0;
      
      if( rStressMeasure == ConstitutiveModelData::StressMeasure_PK2 ){ //Variables.Strain.Matrix = RightCauchyGreen (C)

	//2nd derivatives
	Dabcd = GetJRightCauchyGreen2ndDerivative(rVariables.Strain,Dabcd,a,b,c,d);
	Cabcd += rVariables.Factors.Alpha4 * Dabcd;
	
	//1st derivatives
	Dabcd = GetJRightCauchyGreenSquare1stDerivative(rVariables.Strain,Dabcd,a,b,c,d);
	Cabcd += rVariables.Factors.Beta4 * Dabcd;

	Cabcd *= 4.0;
      }
      else if( rStressMeasure == ConstitutiveModelData::StressMeasure_Kirchhoff ){ //Variables.Strain.Matrix = LeftCauchyGreen (b)
	//2nd derivatives
	Dabcd = GetJLeftCauchyGreen2ndDerivative(rVariables.Strain,Dabcd,a,b,c,d);
	Cabcd += rVariables.Factors.Alpha4 * Dabcd;
	
	//1st derivatives
	Dabcd = GetJLeftCauchyGreenSquare1stDerivative(rVariables.Strain,Dabcd,a,b,c,d);
	Cabcd += rVariables.Factors.Beta4 * Dabcd;

	Cabcd *= 4.0;
	
      }	

      rCabcd += Cabcd;
      
      return rCabcd;
      
      KRATOS_CATCH(" ")
    }
    
    

    virtual void CalculateScalingFactors(HyperElasticDataType& rVariables) override
    {
      KRATOS_TRY
	  
      rVariables.Factors.Alpha4 = this->GetVolumetricFunction1stJDerivative(rVariables,rVariables.Factors.Alpha4);
      rVariables.Factors.Beta4  = this->GetVolumetricFunction2ndJDerivative(rVariables,rVariables.Factors.Beta4);
		
      KRATOS_CATCH(" ")
    }
    

    //************// W
    
    virtual void CalculateAndAddIsochoricStrainEnergy(HyperElasticDataType& rVariables, double& rIsochoricDensityFunction) override
    {
      KRATOS_TRY

      const MaterialDataType& rMaterial = rVariables.GetMaterialParameters();

      const std::vector<double>& rModelParameters = rMaterial.GetModelParameters(); //nu values, lambda values

      unsigned int size = (rModelParameters.size()/2.0);	

      for(unsigned int p=0; p<size; p++)
      {
	  const double& mu_p = rModelParameters[p];
	  const double& alpha_p = rModelParameters[p+size];
	  rIsochoricDensityFunction += (mu_p/alpha_p) * ( std::pow(rVariables.Strain.Eigen.Values[0],alpha_p) + std::pow(rVariables.Strain.Eigen.Values[1],alpha_p) + std::pow(rVariables.Strain.Eigen.Values[2],alpha_p) - 3.0 );
      }
      
      KRATOS_CATCH(" ")
    }
    
    
    virtual void CalculateAndAddVolumetricStrainEnergy(HyperElasticDataType& rVariables, double& rVolumetricDensityFunction) override
    {
      KRATOS_TRY

      const MaterialDataType& rMaterial = rVariables.GetMaterialParameters();

      //energy function "U(J) = (K/2)*(lnJ)²"
      rVolumetricDensityFunction += rMaterial.GetBulkModulus() * 0.5 * pow(std::log(rVariables.Strain.Invariants.J),2);
	
      KRATOS_CATCH(" ")
    }

    //************// dW
    
    virtual double& GetVolumetricFunction1stJDerivative(HyperElasticDataType& rVariables, double& rDerivative) override //dU/dJ
    {
      KRATOS_TRY
	
      // const ModelDataType&  rValues = rVariables.GetModelData();
      
      // rDerivative = rValues.GetPressure();

      // return rDerivative;

      const MaterialDataType& rMaterial = rVariables.GetMaterialParameters();

      //derivative of "U(J) = (K/2)*ln(J)²"
      //dU(J)/dJ = (K)*(lnJ/J)
      rDerivative = rMaterial.GetBulkModulus() * std::log( rVariables.Strain.Invariants.J );

      rDerivative /= rVariables.Strain.Invariants.J;
      
      return rDerivative;
      
      KRATOS_CATCH(" ")
    };


    virtual double& GetVolumetricFunction2ndJDerivative(HyperElasticDataType& rVariables, double& rDerivative) override //ddU/dJdJ
    {
      KRATOS_TRY

      // rDerivative = 0.0;

      // return rDerivative;
	  
      const MaterialDataType& rMaterial = rVariables.GetMaterialParameters();

      //derivative of "dU(J)/dJ = (K)*(lnJ/J)"
      //ddU(J)/dJdJ = (K)*(1-lnJ)/J²
      rDerivative = rMaterial.GetBulkModulus() * (1.0 -std::log(rVariables.Strain.Invariants.J)) / (rVariables.Strain.Invariants.J * rVariables.Strain.Invariants.J);

      return rDerivative;
      
      KRATOS_CATCH(" ")
    };

    
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
	

    ///@}
    ///@name Private Operators
    ///@{


    ///@}
    ///@name Private Operations
    ///@{


    ///@}
    ///@name Private  Access
    ///@{

	
    ///@}
    ///@name Serialization
    ///@{
    friend class Serializer;


    virtual void save(Serializer& rSerializer) const override
    {
      KRATOS_SERIALIZE_SAVE_BASE_CLASS( rSerializer, OgdenModel )
    }

    virtual void load(Serializer& rSerializer) override
    {
      KRATOS_SERIALIZE_LOAD_BASE_CLASS( rSerializer, OgdenModel )      
    }

    ///@}
    ///@name Private Inquiry
    ///@{


    ///@}
    ///@name Un accessible methods
    ///@{

    ///@}

  }; // Class IsochoricOgdenModel

  ///@}

  ///@name Type Definitions
  ///@{


  ///@}
  ///@name Input and output
  ///@{


  ///@}

  ///@} addtogroup block

}  // namespace Kratos.

#endif // KRATOS_ISOCHORIC_OGDEN_MODEL_H_INCLUDED  defined 


