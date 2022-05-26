#pragma once 


#include <vulkan/vulkan.hpp>
#include <string>
#include <iostream>


struct ShadercHelper {

    // Returns GLSL shader source text after preprocessing.
    static std::string preprocess_shader(const std::string& source_name,
        shaderc_shader_kind kind,
        const std::string& source) {
        shaderc::Compiler compiler;
        shaderc::CompileOptions options;

        // Like -DMY_DEFINE=1
        options.AddMacroDefinition("MY_DEFINE", "1");

        shaderc::PreprocessedSourceCompilationResult result =
            compiler.PreprocessGlsl(source, kind, source_name.c_str(), options);

        if (result.GetCompilationStatus() != shaderc_compilation_status_success) {
            std::cerr << result.GetErrorMessage();
            return "";
        }

        return { result.cbegin(), result.cend() };
    }

    // Compiles a shader to SPIR-V assembly. Returns the assembly text
    // as a string.
    static std::string compile_file_to_assembly(const std::string& source_name,
        shaderc_shader_kind kind,
        const std::string& source,
        bool optimize = false) {
        shaderc::Compiler compiler;
        shaderc::CompileOptions options;

        // Like -DMY_DEFINE=1
        options.AddMacroDefinition("MY_DEFINE", "1");
        if (optimize) options.SetOptimizationLevel(shaderc_optimization_level_size);

        shaderc::AssemblyCompilationResult result = compiler.CompileGlslToSpvAssembly(
            source, kind, source_name.c_str(), options);

        if (result.GetCompilationStatus() != shaderc_compilation_status_success) {
            std::cerr << result.GetErrorMessage();
            return "";
        }

        return { result.cbegin(), result.cend() };
    }

    // Compiles a shader to a SPIR-V binary. Returns the binary as
    // a vector of 32-bit words.
    static std::vector<uint32_t> compile_file(const std::string& source_name,
        shaderc_shader_kind kind,
        const std::string& source,
        bool optimize = false) {
        shaderc::Compiler compiler;
        shaderc::CompileOptions options;

        // Like -DMY_DEFINE=1
        //options.AddMacroDefinition("MY_DEFINE", "1");
        if (optimize) options.SetOptimizationLevel(shaderc_optimization_level_size);

        shaderc::SpvCompilationResult module =
            compiler.CompileGlslToSpv(source, kind, source_name.c_str(), options);

        if (module.GetCompilationStatus() != shaderc_compilation_status_success) {
            std::cerr << module.GetErrorMessage();
            return std::vector<uint32_t>();
        }

        return { module.cbegin(), module.cend() };
    }

    /// <summary>
    /// Return corresponding shaderc flag for shader from provided vulkan shader stage flag
    /// </summary>
    /// <param name="vkShaderStage"></param>
    /// <returns></returns>
    static shaderc_shader_kind GetShaderCStageFlag(vk::ShaderStageFlagBits vkShaderStage) {
        if (vkShaderStage == vk::ShaderStageFlagBits::eVertex) {
            return shaderc_shader_kind::shaderc_vertex_shader; 
        }
        else if (vkShaderStage == vk::ShaderStageFlagBits::eFragment) {
            return shaderc_shader_kind::shaderc_fragment_shader; 
        }
        else {
            throw std::runtime_error("Invalid option -- ShadercHelper"); 
        }
    }
};