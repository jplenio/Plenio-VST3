#include "ara/factory.h"
#include "ara/document_controller.h"
#include "plugin/ids.h"
#include "plenio_version.h"

#include <array>

namespace plenio::ara
{
namespace
{
class FactoryConfig : public ARA::PlugIn::FactoryConfig
{
public:
    ARA::ARAAPIGeneration getLowestSupportedApiGeneration() const noexcept override { return ARA::kARAAPIGeneration_2_0_Final; }
    ARA::ARAAPIGeneration getHighestSupportedApiGeneration() const noexcept override { return ARA::kARAAPIGeneration_2_3_Final; }

    const char* getFactoryID() const noexcept override { return araFactoryId; }
    const char* getPlugInName() const noexcept override { return araName; }
    const char* getManufacturerName() const noexcept override { return vendorName; }
    const char* getInformationURL() const noexcept override { return vendorUrl; }
    const char* getVersion() const noexcept override { return PLENIO_VERSION_NUMBERS; }
    const char* getDocumentArchiveID() const noexcept override { return araDocumentArchiveId; }

    ARA::ARASize getAnalyzeableContentTypesCount() const noexcept override { return 0; }
    const ARA::ARAContentType* getAnalyzeableContentTypes() const noexcept override { return nullptr; }
};
} // namespace

const ARA::ARAFactory* factory() noexcept
{
    return ARA::PlugIn::PlugInEntry::getPlugInEntry<FactoryConfig, DocumentController>()->getFactory();
}

MainFactory::MainFactory()
{
    FUNKNOWN_CTOR
}

MainFactory::~MainFactory()
{
    FUNKNOWN_DTOR
}

Steinberg::FUnknown* MainFactory::createInstance (void*)
{
    return static_cast<ARA::IMainFactory*> (new MainFactory());
}

const ARA::ARAFactory* PLUGIN_API MainFactory::getFactory()
{
    return factory();
}

IMPLEMENT_FUNKNOWN_METHODS (MainFactory, ARA::IMainFactory, ARA::IMainFactory::iid)

} // namespace plenio::ara
