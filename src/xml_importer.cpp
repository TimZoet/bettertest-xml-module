#include "bettertest_xml/xml_importer.h"

////////////////////////////////////////////////////////////////
// External includes.
////////////////////////////////////////////////////////////////

#include "pugixml.hpp"

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "bettertest/exceptions/import_error.h"
#include "bettertest/suite/suite_data.h"
#include "bettertest/suite/test_data.h"
#include "bettertest/suite/test_suite.h"

namespace bt
{
    void fromXml(const pugi::xml_node& node, SuiteData& s)
    {
        s.dateCreated = node.attribute("dateCreated").as_string();
        s.dateLastRun = node.attribute("dateLastRun").as_string();
        s.name        = node.attribute("name").as_string();
        s.passing     = node.attribute("passing").as_bool();
        s.runIndex    = node.attribute("runIndex").as_ullong();
        s.version     = node.attribute("version").as_string();
    }

    void fromXml(const pugi::xml_node& node, TestData& t)
    {
        t.dateCreated = node.attribute("dateCreated").as_string();
        t.dateLastRun = node.attribute("dateLastRun").as_string();
        t.name        = node.attribute("name").value();
        t.passing     = node.attribute("passing").as_bool();
    }

    XmlImporter::XmlImporter(std::filesystem::path directory) : IImporter(std::move(directory)) {}

    bool XmlImporter::readSuiteFile(TestSuite& suite)
    {
        if (!exists(path / "suite.xml")) return false;

        // Read XML data from file.
        pugi::xml_document doc;
        if (const auto res = doc.load_file((path / "suite.xml").c_str()); !res)
            throw ImportError("Failed to open suite file");

        // Read suite data.
        fromXml(doc.child("suite"), suite.getData());

        // Read unit test states.
        auto& unitTests = suite.getUnitTestSuite().getData();
        for (auto& node : doc.child("unitTests").children("testState"))
        {
            unitTests.emplace_back(std::make_unique<TestData>());
            fromXml(node, *unitTests.back());
        }

        return true;
    }
}  // namespace bt