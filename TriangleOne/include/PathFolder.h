#pragma once

#include <windows.h>
#include <filesystem>

static std::filesystem::path GetExeDir() {
	char buffer[MAX_PATH];
	GetModuleFileNameA(NULL, buffer, MAX_PATH);
	return std::filesystem::path(buffer).parent_path();
}
const std::filesystem::path SHADER_DIR = GetExeDir() / ".." / ".." / "TriangleOne" / "Shader";
const std::filesystem::path ASSETS_DIR = GetExeDir() / ".." / ".." / "Assets";
const std::filesystem::path SOLUTION_DIR = GetExeDir() / ".." / "..";