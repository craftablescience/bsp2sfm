// ReSharper disable CppDFATimeOver
// ReSharper disable CppDeclaratorNeverUsed
// ReSharper disable CppParameterMayBeConst
// ReSharper disable CppRedundantQualifier
// ReSharper disable CppUseStructuredBinding

#include <chrono>
#include <cstdio>
#include <cstdlib>
#include <filesystem>
#include <format>
#include <exception>
#include <memory>
#include <random>
#include <ranges>
#include <string>
#include <string_view>

#if defined(_WIN32)
#include <io.h>
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif

#include <argparse/argparse.hpp>
#include <bsppp/bsppp.h>
#include <sourcepp/FS.h>
#include <vtfpp/vtfpp.h>

#include "Config.hpp"

using namespace sourcepp;
using namespace std::literals;

int main(int argc, const char* const argv[]) {
#ifdef _WIN32
	SetConsoleOutputCP(CP_UTF8); // Set up console to show UTF-8 characters
	setvbuf(stdout, nullptr, _IOFBF, 1000); // Enable buffering so VS terminal won't chop up UTF-8 byte sequences
#endif

	argparse::ArgumentParser cli{PROJECT_NAME, PROJECT_VERSION, argparse::default_arguments::help};

#ifdef _WIN32
	// Add the Windows-specific ones because why not
	cli.set_prefix_chars("-/");
	cli.set_assign_chars("=:");
#endif

	try {
		//region General Arguments

		std::string inputPath;
		cli
			.add_argument("path")
			.metavar("PATH")
			.help("The path to the input map.")
			.required();

		//endregion

		//region Program Info

		static constexpr std::string_view PROGRAM_DETAILS{
			"Program details:\n\n"
			PROJECT_NAME_PRETTY " — version v" PROJECT_VERSION " — created by " PROJECT_ORGANIZATION_NAME " — licensed under MIT\n"
			"See the project README for more information.\n"
			"Want to report a bug or request a feature? Make an issue at " PROJECT_GITHUB_URL "/issues"
		};

		cli.add_epilog(std::string{PROGRAM_DETAILS});

		//endregion

		cli.parse_args(argc, argv);

		// Check input path
		if (inputPath.empty() || !std::filesystem::exists(inputPath)) {
			throw std::invalid_argument{"Input map does not exist!"};
		}

		// Decompress lumps
		{
			bsppp::BSP bsp{inputPath};
			if (!bsp) {
				throw std::invalid_argument{"\"" + inputPath + "\" is not a valid map!"};
			}
			for (std::underlying_type_t<bsppp::BSPLump> i = 0; i < 64; i++) {
				if (
					const auto lumpID = static_cast<bsppp::BSPLump>(i);
					bsp.isLumpCompressed(lumpID)
				) {
					if (const auto lumpData = bsp.getLumpData(lumpID)) {
						bsp.setLump(lumpID, bsp.getLumpVersion(lumpID), *lumpData);
					} else {
						throw std::invalid_argument{std::format("Failed to decompress lump {}!", i)};
					}
				}
			}
			if (!bsp.bake()) {
				throw std::runtime_error{"Failed to save map at \"" + inputPath + "\"!"};
			}
		}

		// Convert cubemaps
		{
			auto pak = bsppp::PakLump::open(inputPath);
			if (!pak) {
				throw std::runtime_error{"Failed to open map paklump at \"" + inputPath + "\"!"};
			}
			std::vector<std::string> cubemapPaths;
			pak->runForAllEntries("materials", [&pak, &cubemapPaths](const std::string& path, const vpkpp::Entry&) {
				if (string::toLower(path).ends_with(".vtf")) {
					if (const auto vtfData = pak->readEntry(path)) {
						if (const vtfpp::VTF vtf{*vtfData}; vtf.getFaceCount() > 6) {
							cubemapPaths.push_back(path);
						}
					}
				}
			});
			for (const auto& cubemapPath : cubemapPaths) {
				if (const auto vtfData = pak->readEntry(cubemapPath)) {
					vtfpp::VTF vtf{*vtfData};
					vtf.setVersion(5);
					pak->addEntry(cubemapPath, vtf.bake());
				}
			}
			if (!pak->bake()) {
				throw std::runtime_error{"Failed to write modified map paklump at \"" + inputPath + "\"!"};
			}
		}
	} catch (const std::exception& e) {
		if (argc > 1) {
			std::cerr << e.what() << '\n' << std::endl;
		} else {
			std::cout << cli << std::endl;
		}
		return EXIT_FAILURE;
	}
	return EXIT_SUCCESS;
}
