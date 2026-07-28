#include "ParticleExporter_TXT.h"
#include <Utilities/Logger.h>
#include <Utilities/FileSystem.h>
#include "SPlisHSPlasH/Simulation.h"
#include "SPlisHSPlasH/FluidModel.h"
#include <SPlisHSPlasH/SurfaceTension/SurfaceTension_Akinci2013.h>
#include <SPlisHSPlasH/WCSPH/SimulationDataWCSPH.h>
#include <SPlisHSPlasH/ICSPH/SimulationDataICSPH.h>
#include <SPlisHSPlasH/IISPH/SimulationDataIISPH.h>
#include <variant>
#include <SPlisHSPlasH/WCSPH/TimeStepWCSPH.h>
#include <SPlisHSPlasH/ICSPH/TimeStepICSPH.h>
#include <SPlisHSPlasH/IISPH/TimeStepIISPH.h>

using namespace SPH;
using namespace Utilities;


static float validValue(float val)
{
	return (std::isnan(val)) ? 0 : val;
}


ParticleExporter_TXT::ParticleExporter_TXT(SimulatorBase *base) :
	ExporterBase(base)
{
}

ParticleExporter_TXT::~ParticleExporter_TXT(void)
{
}

void ParticleExporter_TXT::init(const std::string& outputPath)
{
	m_exportPath = FileSystem::normalizePath(outputPath + "/txt");
}

void ParticleExporter_TXT::step(const unsigned int frame)
{
	if (!m_active)
		return;

	Simulation* sim = Simulation::getCurrent();
	for (unsigned int i = 0; i < sim->numberOfFluidModels(); i++)
	{
		FluidModel* model = sim->getFluidModel(i);
		std::string fileName = "ParticleData_" + model->getId() + "_" + std::to_string(frame);
		std::string exportFileName = FileSystem::normalizePath(m_exportPath + "/" + fileName);
		writeParticles(exportFileName + ".txt", model);
	}
}

void ParticleExporter_TXT::reset()
{
}

void ParticleExporter_TXT::setActive(const bool active)
{
	ExporterBase::setActive(active);
	if (m_active)
		FileSystem::makeDirs(m_exportPath);
}

void ParticleExporter_TXT::createParticleFile(const std::string& fileName, FluidModel* model)
{
	// Open the file
	m_outfile = new std::ofstream(fileName);
	if (!m_outfile->is_open())
	{
		LOG_WARN << "Cannot open a file to save txt particles.";
		return;
	}

	Simulation* sim = Simulation::getCurrent();
	TimeStep* timeStep = sim->getTimeStep();

	//DO NOT MODIFY THIS LINE. It is needed to track the files' version
	(*m_outfile) << "EXPORTER VERSION" << "\n"
				 << CURRENT_VERSION << "\n";
	
	(*m_outfile) << "SimulationMethod: " << timeStep->getMethodName() << "\n"
				 << "NbrRigidBodies: " << sim->numberOfBoundaryModels() << "\n";
}

void ParticleExporter_TXT::writeParticles(const std::string& fileName, FluidModel* model, const unsigned int objId)
{
	createParticleFile(fileName, model);

	if (m_outfile == nullptr)
	{
		LOG_WARN << "No file found to write .txt particles' data.";
		return;
	}

	//Write all info to file
	const unsigned int numParticles = model->numActiveParticles();

	(*m_outfile) << "NbrParticles: " << numParticles << "\n";

	m_outfile->precision(9);
	m_outfile->setf(std::ios::fixed);

	//Check if surfaceTension is Akinci2013
	//	This is needed to get the normal of each particule
	SurfaceTension_Akinci2013* surfaceTension = static_cast<SurfaceTension_Akinci2013*>(model->getSurfaceTensionBase());
	if (surfaceTension == nullptr)
	{
		setActive(false);
		std::cout << "[FAILED EXPORT TXT] Could not export normals, SurfaceTension Akinci2013 needs to be activated.\n";
		return;
	}

	TimeStep* timeStep = Simulation::getCurrent()->getTimeStep();

	struct SimulationData
	{
		SimulationMethods method = (SimulationMethods)Simulation::getCurrent()->getSimulationMethod();
		SimulationDataWCSPH WCSPH{};
		SimulationDataICSPH ICSPH{};
		SimulationDataIISPH IISPH{};

		SimulationData() 
		{
			//Need to be init to not break when using their destructor
			WCSPH.init();
			ICSPH.init();
			IISPH.init();
		}
	} simulationData;


	switch (simulationData.method)
	{
		case SimulationMethods::WCSPH: 
		{
			TimeStepWCSPH* preciseTimeStep = static_cast<TimeStepWCSPH*>(timeStep);
			if (preciseTimeStep == nullptr) 
			{
				setActive(false);
				std::cout << "[FAILED EXPORT TXT] Could not export pressure, errror when exporting WCSPH simulation data\n";
				return;
			}

			simulationData.WCSPH = preciseTimeStep->getSimulationData();
			break;
		}
		case SimulationMethods::ICSPH: 
		{
			TimeStepICSPH* preciseTimeStep = static_cast<TimeStepICSPH*>(timeStep);
			if (preciseTimeStep != nullptr)
			{
				setActive(false);
				std::cout << "[FAILED EXPORT TXT] Could not export pressure, errror when exporting ICSPH simulation data\n";
				return;
			}

			simulationData.ICSPH = preciseTimeStep->getSimulationData();
			break;
		}
		case SimulationMethods::IISPH:
		{
			TimeStepIISPH* preciseTimeStep = static_cast<TimeStepIISPH*>(timeStep);
			if (preciseTimeStep == nullptr)
			{
				setActive(false);
				std::cout << "[FAILED EXPORT TXT] Could not export pressure, errror when exporting IISPH simulation data\n";
				return;
			}

			simulationData.IISPH = preciseTimeStep->getSimulationData();
			break;
		}

		default:
		{
			setActive(false);
			std::cout << "[FAILED EXPORT TXT] Could not export pressure, Only WCSPH, ICSPH and IISPH are supported.\n";
			return;
		}
	}

	for (unsigned int i = 0; i < numParticles; i++)
	{
		const Vector3r& pos = model->getPosition(i);
		const Vector3r& vel = model->getVelocity(i);
		const Vector3r& accel = model->getAcceleration(i);
		const Vector3r& normal = surfaceTension->getNormal(i);
		Real pressure;

		switch (simulationData.method)
		{
			case SimulationMethods::WCSPH: 
			{
				pressure = simulationData.WCSPH.getPressure(0 , i);
				break;
			}
			case SimulationMethods::ICSPH:
			{
				pressure = simulationData.ICSPH.getPressure(0 , i);
				break;
			}
			case SimulationMethods::IISPH:
			{
				pressure = simulationData.IISPH.getPressure(0 , i);
				break;
			}
			default:
				assert(false);	//If this happens, it means that a simulation method is not supported when it should
				return;
		}

		(*m_outfile)
			<< validValue(pos[0]) << ' '
			<< validValue(pos[1]) << ' '
			<< validValue(pos[2]) << ' '

			<< validValue(normal[0]) << ' '
			<< validValue(normal[1]) << ' '
			<< validValue(normal[2]) << ' '

			<< validValue(vel[0]) << ' '
			<< validValue(vel[1]) << ' '
			<< validValue(vel[2]) << ' '

			<< validValue(accel[0]) << ' '
			<< validValue(accel[1]) << ' '
			<< validValue(accel[2]) << ' '

			<< model->getDensity(i) << ' '
			<< pressure << ' '
			<< '\n';
	}

	m_outfile->close();
	delete m_outfile;
	m_outfile = nullptr;
}
