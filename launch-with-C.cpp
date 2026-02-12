#include <iostream>
#include <string>
#include <windows.h>
#include <fstream>

// 读取配置文件函数
std::string read_target_directory() {
    std::ifstream file("config.json");
    if (!file.is_open()) {
        std::cout << "无法打开配置文件" << std::endl;
        return "";
    }
    
    std::string line;
    std::string target_dir = "";
    
    while (std::getline(file, line)) {
        if (line.find("\"directory\"") != std::string::npos) {
            size_t start = line.find('\"', line.find(':') + 1) + 1;
            size_t end = line.rfind('\"');
            if (start != std::string::npos && end != std::string::npos) {
                target_dir = line.substr(start, end - start);
                break;
            }
        }
    }
    file.close();
    return target_dir;
}

// 递归复制目录函数
bool copy_directory_recursive(const std::string& source, const std::string& destination) {
    // 创建目标目录
    CreateDirectoryA(destination.c_str(), NULL);
    
    WIN32_FIND_DATAA find_data;
    std::string search_path = source + "\\*";
    HANDLE h_find = FindFirstFileA(search_path.c_str(), &find_data);
    
    if (h_find == INVALID_HANDLE_VALUE) {
        return false;
    }
    
    bool success = true;
    
    do {
        // 跳过.和..目录项
        if (strcmp(find_data.cFileName, ".") == 0 || strcmp(find_data.cFileName, "..") == 0) {
            continue;
        }
        
        std::string src_path = source + "\\" + find_data.cFileName;
        std::string dest_path = destination + "\\" + find_data.cFileName;
        
        if (find_data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
            // 递归复制子目录
            if (!copy_directory_recursive(src_path, dest_path)) {
                success = false;
            }
        } else {
            // 复制文件，同名文件覆盖
            if (!CopyFileA(src_path.c_str(), dest_path.c_str(), FALSE)) {
                std::cout << "复制文件失败: " << src_path << std::endl;
                success = false;
            }
        }
    } while (FindNextFileA(h_find, &find_data));
    
    FindClose(h_find);
    return success;
}

// 复制B2C目录内容到目标目录根目录的函数
bool copy_b2c_to_root() {
    std::string target_directory = read_target_directory();
    if (target_directory.empty()) {
        std::cout << "未找到有效的目标目录配置" << std::endl;
        return false;
    }
    
    // 获取当前工作目录
    char current_dir[MAX_PATH];
    GetCurrentDirectoryA(MAX_PATH, current_dir);
    
    // 构建B2C目录的完整路径
    std::string b2c_path = std::string(current_dir) + "\\B2C";
    
    std::cout << "正在从 " << b2c_path << " 复制到 " << target_directory << std::endl;
    
    bool result = copy_directory_recursive(b2c_path, target_directory);

    return result;
}

int main() {
    #ifdef _WIN32
    SetConsoleOutputCP(65001);  // 设置控制台为UTF-8
    #endif
    std::cout << "开始复制B2C目录内容到目标根目录..." << std::endl;

    // 执行复制操作（无论成功与否都继续）
    copy_b2c_to_root();

    // 复制完成后检查并启动游戏
    std::string target_directory = read_target_directory();
    if (!target_directory.empty()) {
        std::string exe_path = "\"" + target_directory + "\\Arknights.exe\"";
        
        std::string raw_exe_path = target_directory + "\\Arknights.exe";
        DWORD attributes = GetFileAttributesA(raw_exe_path.c_str());
        if (attributes == INVALID_FILE_ATTRIBUTES) {
            DWORD error = GetLastError();
            std::cout << "错误: 找不到文件 " << raw_exe_path << std::endl;
            std::cout << "错误代码: " << error << std::endl;
            std::cout << "请确认目标目录中包含Arknights.exe文件" << std::endl;
            return 1;
        }
        // 检查是否为可执行文件
        if (attributes & FILE_ATTRIBUTE_DIRECTORY) {
            std::cout << "错误: " << raw_exe_path << " 是目录而不是可执行文件" << std::endl;
            return 1;
        }
        
        std::cout << "准备启动: " << exe_path << std::endl;
        std::cout << "正在启动Arknights.exe..." << std::endl;
        
        // 运行Arknights.exe - 使用带引号的路径
        int result = system(exe_path.c_str());
        if (result == 0) {
            std::cout << "Arknights.exe启动成功!" << std::endl;
        } else {
            std::cout << "Arknights.exe启动失败，返回代码: " << result << std::endl;
            std::cout << "可能的原因:" << std::endl;
            std::cout << "1. 文件损坏或不完整" << std::endl;
            std::cout << "2. 缺少必要的依赖文件" << std::endl;
            std::cout << "3. 权限不足" << std::endl;
        }
    }
    return 0;
}