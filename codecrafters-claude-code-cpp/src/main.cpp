#include <cstdlib>
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <cstdio>

#include <cpr/cpr.h>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

int main(int argc, char* argv[]) {

    if (argc < 3 || std::string(argv[1]) != "-p") {
        std::cerr << "Expected first argument to be '-p'" << std::endl;
        return 1;
    }

    std::string prompt = argv[2];

    if (prompt.empty()) {
        std::cerr << "Prompt must not be empty" << std::endl;
        return 1;
    }

    const char* api_key_env = std::getenv("OPENROUTER_API_KEY");
    const char* base_url_env = std::getenv("OPENROUTER_BASE_URL");

    std::string api_key = api_key_env ? api_key_env : "";
    std::string base_url = base_url_env ? base_url_env : "https://openrouter.ai/api/v1";

    if (api_key.empty()) {
        std::cerr << "OPENROUTER_API_KEY is not set" << std::endl;
        return 1;
    }

    // Conversation history
    json messages = json::array({
        {
            {"role", "user"},
            {"content", prompt}
        }
    });

    // Agent loop
    while (true) {

        json request_body = {
            {"model", "anthropic/claude-haiku-4.5"},
            {"messages", messages},
            {"tools", json::array({

                {
                    {"type", "function"},
                    {"function", {
                        {"name", "Read"},
                        {"description", "Read and return the contents of a file"},
                        {"parameters", {
                            {"type", "object"},
                            {"properties", {
                                {"file_path", {
                                    {"type", "string"},
                                    {"description", "Path to the file"}
                                }}
                            }},
                            {"required", json::array({"file_path"})}
                        }}
                    }}
                },

                {
                    {"type", "function"},
                    {"function", {
                        {"name", "Write"},
                        {"description", "Write content to a file"},
                        {"parameters", {
                            {"type", "object"},
                            {"required", json::array({"file_path","content"})},
                            {"properties", {
                                {"file_path", {
                                    {"type", "string"},
                                    {"description", "File path"}
                                }},
                                {"content", {
                                    {"type", "string"},
                                    {"description", "Content to write"}
                                }}
                            }}
                        }}
                    }}
                },

                {
                    {"type", "function"},
                    {"function", {
                        {"name", "Bash"},
                        {"description", "Execute a shell command"},
                        {"parameters", {
                            {"type", "object"},
                            {"required", json::array({"command"})},
                            {"properties", {
                                {"command", {
                                    {"type", "string"},
                                    {"description", "The command to execute"}
                                }}
                            }}
                        }}
                    }}
                }

            })}
        };

        cpr::Response response = cpr::Post(
            cpr::Url{base_url + "/chat/completions"},
            cpr::Header{
                {"Authorization", "Bearer " + api_key},
                {"Content-Type", "application/json"}
            },
            cpr::Body{request_body.dump()}
        );

        if (response.status_code != 200) {
            std::cerr << "HTTP error: " << response.status_code << std::endl;
            return 1;
        }

        json result = json::parse(response.text);

        if (!result.contains("choices") || result["choices"].empty()) {
            std::cerr << "No choices in response" << std::endl;
            return 1;
        }

        auto message = result["choices"][0]["message"];

        // Save assistant response
        messages.push_back(message);

        // Check for tool calls
        if (message.contains("tool_calls") && !message["tool_calls"].empty()) {

            for (auto& tool_call : message["tool_calls"]) {

                std::string tool_id = tool_call["id"];
                std::string function_name = tool_call["function"]["name"];
                std::string arguments_str = tool_call["function"]["arguments"];

                json args = json::parse(arguments_str);

                std::string tool_result;

                // READ TOOL
                if (function_name == "Read") {

                    std::string file_path = args["file_path"];

                    std::ifstream file(file_path);

                    if (!file.is_open()) {
                        tool_result = "Error: Unable to open file";
                    } else {
                        std::stringstream buffer;
                        buffer << file.rdbuf();
                        tool_result = buffer.str();
                    }
                }

                // WRITE TOOL
                else if (function_name == "Write") {

                    std::string file_path = args["file_path"];
                    std::string content = args["content"];

                    std::ofstream file(file_path);

                    if (!file.is_open()) {
                        tool_result = "Error: Unable to write file";
                    } else {
                        file << content;
                        file.close();
                        tool_result = "File written successfully";
                    }
                }

                // BASH TOOL
                else if (function_name == "Bash") {

                    std::string command = args["command"];

                    std::string full_command = command + " 2>&1";

                    FILE* pipe = popen(full_command.c_str(), "r");

                    if (!pipe) {
                        tool_result = "Error: failed to execute command";
                    } else {

                        char buffer[256];
                        std::string output;

                        while (fgets(buffer, sizeof(buffer), pipe) != nullptr) {
                            output += buffer;
                        }

                        pclose(pipe);

                        tool_result = output;
                    }
                }

                // Add tool result to conversation
                messages.push_back({
                    {"role", "tool"},
                    {"tool_call_id", tool_id},
                    {"content", tool_result}
                });
            }

            continue;
        }

        // No tools → final answer
        if (message.contains("content") && !message["content"].is_null()) {
            std::cout << message["content"].get<std::string>() << std::endl;
        }

        break;
    }

    return 0;
}