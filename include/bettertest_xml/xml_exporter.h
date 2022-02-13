#pragma once

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "bettertest/output/exporter_interface.h"

namespace bt
{
    class XmlExporter final : public IExporter
    {
    public:
        static constexpr char type[] = "xml";

        XmlExporter() = delete;

        XmlExporter(const XmlExporter&) = delete;

        XmlExporter(XmlExporter&&) = delete;

        explicit XmlExporter(std::filesystem::path directory);

        ~XmlExporter() noexcept override;

        XmlExporter& operator=(const XmlExporter&) = delete;

        XmlExporter& operator=(XmlExporter&&) = delete;

        void writeSuiteFile(const TestSuite& suite) override;

        void writeUnitTestFile(const TestSuite& suite, const IUnitTest& test, const std::string& name) override;

        [[nodiscard]] bool supportsMultithreading() const noexcept override;
    };
}  // namespace bt
