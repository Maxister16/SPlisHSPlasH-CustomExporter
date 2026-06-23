#ifndef __ParticleExporter_TXT_h__
#define __ParticleExporter_TXT_h__

#include "ExporterBase.h"
#include "SPlisHSPlasH/FluidModel.h"
#include <future>

namespace SPH
{
	/** \brief Custom particle exporter for the txt format.
	*/
	class ParticleExporter_TXT : public ExporterBase
	{
	protected: 
		std::string m_exportPath;
		std::string m_particleFile;
		std::future<void> m_handle;

		void writeParticlesTXT(const std::string& fileName, FluidModel* model, const unsigned int objId=0xffffffff);

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
