#ifndef __ParticleExporter_TXT_h__
#define __ParticleExporter_TXT_h__

#include "ExporterBase.h"
#include <future>
#include <SPlisHSPlasH/Simulation.h>

namespace SPH
{
	/** \brief Custom particle exporter for the txt format.
	*/
	class ParticleExporter_TXT : public ExporterBase
	{
	protected:
		const int CURRENT_VERSION = 4;
		std::string m_exportPath;
		std::ofstream* m_outfile;

		void createParticleFile(const std::string& fileName, FluidModel* model);
		void writeParticles(const std::string& fileName, FluidModel* model, const unsigned int objId = 0xffffffff);

	public:
		ParticleExporter_TXT(SimulatorBase *base);
		ParticleExporter_TXT(const ParticleExporter_TXT&) = delete;
        ParticleExporter_TXT& operator=(const ParticleExporter_TXT&) = delete;
		virtual ~ParticleExporter_TXT(void);

		virtual void init(const std::string& outputPath);
		virtual void step(const unsigned int frame);
		virtual void reset();
		virtual void setActive(const bool active); 
	};
}

#endif
