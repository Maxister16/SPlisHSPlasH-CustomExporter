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


void ParticleExporter_TXT::writeParticlesTXT(const std::string& fileName, FluidModel* model, const unsigned int objId)
{
	
}