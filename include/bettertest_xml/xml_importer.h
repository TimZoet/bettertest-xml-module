#pragma once

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "bettertest/output/importer_interface.h"

namespace bt
{
    class XmlImporter final : public IImporter
    {
    public:
        static constexpr char type[] = "xml";

        XmlImporter() = delete;

        XmlImporter(const XmlImporter&) = delete;

        XmlImporter(XmlImporter&&) = delete;

        explicit XmlImporter(std::filesystem::path directory);

        ~XmlImporter() noexcept override = default;

        XmlImporter& operator=(const XmlImporter&) = delete;

        XmlImporter& operator=(XmlImporter&&) = delete;

        bool readSuite(TestSuite& suite) override;
    };
}
