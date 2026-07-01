#include "ParticleExporter_TXT.h"
#include <Utilities/Logger.h>
#include <Utilities/FileSystem.h>
#include "SPlisHSPlasH/Simulation.h"

using namespace SPH;
using namespace Utilities;

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

	(*m_outfile) << numParticles << "\n";

	m_outfile->precision(9);
	m_outfile->setf(std::ios::fixed);

	for (unsigned int i = 0; i < numParticles; i++)
	{
		const Vector3r& pos = model->getPosition(i);
		const Vector3r& vel = model->getVelocity(i);
		const Vector3r& accel = model->getAcceleration(i);

		(*m_outfile)
			<< pos[0] << ' '
			<< pos[1] << ' '
			<< pos[2] << ' '

			<< vel[0] << ' '
			<< vel[1] << ' '
			<< vel[2] << ' '

			<< accel[0] << ' '
			<< accel[1] << ' '
			<< accel[2] << ' '

			<< model->getDensity(i) << ' '
			<< '\n';
	}

	m_outfile->close();
	delete m_outfile;
	m_outfile = nullptr;
}
