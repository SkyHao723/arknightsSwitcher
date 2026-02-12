#include <iostream>
#include <fstream>
#include <string>
#include <windows.h>

// 检查配置文件是否存在
bool config_exists(const std::string& config_path) {
    std::ifstream file(config_path);
    return file.good();
}

// 创建默认配置文件
void create_default_config(const std::string& config_path) {
    std::ofstream file(config_path);
    if (file.is_open()) {
        file << "{\n";
        file << "    \"directory\": \"\"\n";
        file << "}";
        file.close();
        std::cout << "已创建默认配置文件: " << config_path << std::endl;
    }
}

struct Config {
    std::string directory;
    int removed;
};

Config read_config(const std::string& config_path) {
    Config config = {"", 0};
    
    // 如果配置文件不存在，创建默认配置
    if (!config_exists(config_path)) {
        create_default_config(config_path);
    }
    
    std::ifstream file(config_path);
    if (!file.is_open()) {
        std::cout << "无法打开配置文件: " << config_path << std::endl;
        return config;
    }
    
    std::string content((std::istreambuf_iterator<char>(file)),
                        std::istreambuf_iterator<char>());
    file.close();
    
    // 简单的JSON解析
    size_t dir_pos = content.find("\"directory\"");
    if (dir_pos != std::string::npos) {
        size_t colon_pos = content.find(':', dir_pos);
        if (colon_pos != std::string::npos) {
            size_t start_quote = content.find('\"', colon_pos);
            size_t end_quote = content.find('\"', start_quote + 1);
            if (start_quote != std::string::npos && end_quote != std::string::npos) {
                config.directory = content.substr(start_quote + 1, end_quote - start_quote - 1);
            }
        }
    }
    return config;
}

void write_config(const std::string& config_path, const Config& config) {
    // 替换路径中的反斜杠为正斜杠
    std::string normalized_path = config.directory;
    for (size_t i = 0; i < normalized_path.length(); ++i) {
        if (normalized_path[i] == '\\') {
            normalized_path[i] = '/';
        }
    }
    
    std::ofstream file(config_path);
    if (file.is_open()) {
        file << "{\n";
        file << "    \"directory\": \"" << normalized_path << "\"\n";
        file << "}";
        file.close();
    }
}
void displayBanner() {
    // 打开banner文件
    std::ifstream file("banner.txt");
    if (!file.is_open()) {
        std::cerr << "无法打开banner.txt文件" << std::endl;
        return;
    }
    
    std::string line;
    
    // 逐行读取并显示
    while (std::getline(file, line)) {
        std::cout << line << std::endl;
        // 每行显示后稍作停顿，营造逐行显示效果
        Sleep(10);
    }
    
    file.close();
}

int main() {
#ifdef _WIN32
    SetConsoleOutputCP(65001);  // 设置控制台为UTF-8
#endif
    displayBanner();
    std::string config_path = "config.json";
    Config config = read_config(config_path);
    
    if (config.directory.empty()) {
        std::cout << "未找到有效目录配置，需要用户输入" << std::endl;
        std::cout << "请输入目标目录路径: ";
        std::getline(std::cin, config.directory);
        write_config(config_path, config);
    } else {
        std::cout << "使用配置文件中的目录: " << config.directory << std::endl;
    }

    std::cout << "请选择启动方式:" << std::endl;
    std::cout << "1. 官服启动" << std::endl;
    std::cout << "2. B服启动" << std::endl;
    std::cout << "请输入选择 (1 或 2): ";
    
    int choice;
    std::cin >> choice;
    
    if (choice == 1) {
        std::cout << "启动官服..." << std::endl;
        system("launch-with-C.exe");
    } else if (choice == 2) {
        std::cout << "启动B服..." << std::endl;
        system("launch-with-B.exe");
    } else {
        std::cout << "无效选择!" << std::endl;
        return 1;
    }
    
    return 0;
}