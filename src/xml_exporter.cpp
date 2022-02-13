#include "bettertest_xml/xml_exporter.h"

////////////////////////////////////////////////////////////////
// Standard includes.
////////////////////////////////////////////////////////////////

#include <algorithm>
#include <format>
#include <fstream>

////////////////////////////////////////////////////////////////
// External includes.
////////////////////////////////////////////////////////////////

#include "pugixml.hpp"

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "bettertest/exceptions/export_error.h"
#include "bettertest/mixins/compare_mixin.h"
#include "bettertest/mixins/exception_mixin.h"
#include "bettertest/suite/suite_data.h"
#include "bettertest/suite/test_suite.h"

namespace bt
{
    void toXml(pugi::xml_node& node, const TestSuite& suite)
    {
        node.append_attribute("dateCreated").set_value(suite.getData().dateCreated.c_str());
        node.append_attribute("dateLastRun").set_value(suite.getData().dateLastRun.c_str());
        node.append_attribute("name").set_value(suite.getName().c_str());
        node.append_attribute("passing").set_value(suite.getData().passing);
        node.append_attribute("runIndex").set_value(suite.getData().runIndex);
        node.append_attribute("version").set_value(suite.getData().version.c_str());
    }

    void toXml(pugi::xml_node& node, const TestData& t)
    {
        auto t_node = node.append_child("testState");
        t_node.append_attribute("dateCreated").set_value(t.dateCreated.c_str());
        t_node.append_attribute("dateLastRun").set_value(t.dateLastRun.c_str());
        t_node.append_attribute("name").set_value(t.name.c_str());
        t_node.append_attribute("passing").set_value(t.passing);
    }

    void toXml(pugi::xml_node& node, const CompareMixin& mixin)
    {
        auto compare = node.append_child(mixin.getName().c_str());

        // Write global stats.
        auto stats = compare.append_child("stats");
        stats.append_attribute("total").set_value(mixin.getComparisons());
        stats.append_attribute("success").set_value(mixin.getSuccesses());
        stats.append_attribute("failure").set_value(mixin.getFailures());
        stats.append_attribute("exception").set_value(mixin.getExceptions());
        
        // Write per-call information.
        auto r = compare.append_child("results");
        std::ranges::for_each(mixin.getResults(), [&r](const auto& res) {
            auto result = r.append_child("result");

            // Write status.
            auto status = result.append_attribute("status");
            switch (res.status)
            {
            case compare_result_t::success: status.set_value("success"); break;
            case compare_result_t::failure: status.set_value("failure"); break;
            case compare_result_t::exception: status.set_value("exception"); break;
            }

            // Write source location.
            auto location = result.append_child("location");
            location.append_attribute("column").set_value(res.location.column());
            location.append_attribute("line").set_value(res.location.line());
            location.append_attribute("file").set_value(res.location.file_name());

            // Write error.
            auto error = result.append_attribute("error");
            error.set_value(res.error.c_str());
        });
    }

    void toXml(pugi::xml_node& node, const ExceptionMixin& mixin)
    {
        auto compare = node.append_child(mixin.getName().c_str());

        // Write global stats.
        auto stats = compare.append_child("stats");
        stats.append_attribute("total").set_value(mixin.getCalls());
        stats.append_attribute("success").set_value(mixin.getSuccesses());
        stats.append_attribute("failure").set_value(mixin.getFailures());

        // Write per-call information.
        auto r = compare.append_child("results");
        std::ranges::for_each(mixin.getResults(), [&r](const auto& res) {
            auto result = r.append_child("result");

            // Write status.
            auto status = result.append_attribute("status");
            status.set_value(res.status ? "success" : "failure");

            // Write source location.
            auto location = result.append_child("location");
            location.append_attribute("column").set_value(res.location.column());
            location.append_attribute("line").set_value(res.location.line());
            location.append_attribute("file").set_value(res.location.file_name());

            // Write error.
            auto error = result.append_attribute("error");
            error.set_value(res.error.c_str());
        });
    }

    XmlExporter::XmlExporter(std::filesystem::path directory) : IExporter(std::move(directory)) {}

    XmlExporter::~XmlExporter() noexcept = default;

    void XmlExporter::writeSuiteFile(const TestSuite& suite)
    {
        // Create directory if it does not exist.
        if (!exists(path)) create_directories(path);

        pugi::xml_document doc;

        // Write suite data.
        auto suite_node = doc.append_child("suite");
        toXml(suite_node, suite);

        // Write unit test states.
        auto ut = doc.append_child("unitTests");
        std::ranges::for_each(suite.getUnitTestSuite().getData().begin(),
                      suite.getUnitTestSuite().getData().end(),
                      [&ut](const auto& t) { toXml(ut, *t); });

        // Write performance test states.
        auto pt = doc.append_child("performanceTests");
        std::ranges::for_each(suite.getPerformanceTestSuite().getData().begin(),
                      suite.getPerformanceTestSuite().getData().end(),
                      [&pt](const auto& t) { toXml(pt, *t); });

        // Write XML data to file.
        std::ofstream file(path / "suite.xml");
        if (!file) throw ExportError("Failed to open suite file");
        doc.save(file);
        file.close();
    }

    void XmlExporter::writeUnitTestFile(const TestSuite& suite, const ITest& test, const std::string& name)
    {
        // Create directory if it does not exist.
        const auto testPath = this->path / name;
        if (!exists(testPath)) create_directories(testPath);

        // Write test results.
        pugi::xml_document doc;
        for (const auto& mixin : test.getMixins())
        {
            if (mixin == CompareMixin::type)
                toXml(doc, dynamic_cast<const CompareMixin&>(test));
            else if (mixin == ExceptionMixin::type)
                toXml(doc, dynamic_cast<const ExceptionMixin&>(test));
            else
            {
                // Unknown mixin type.
            }
        }

        // Generate filename as "unit_########.xml".
        const std::string filename = std::format("unit_{:0>8}.xml", suite.getData().runIndex);

        // Write XML data to file.
        std::ofstream file(testPath / filename);
        if (!file) throw ExportError("Failed to open unit test file");
        doc.save(file);
        file.close();
    }

    bool XmlExporter::supportsMultithreading() const noexcept { return true; }

}  // namespace bt