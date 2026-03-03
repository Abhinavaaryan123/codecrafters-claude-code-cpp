# AI CLI Agent (C++)

A lightweight **AI command-line agent built in C++** that integrates with an LLM via **OpenRouter** and supports **tool calling** to autonomously perform tasks such as reading files, writing files, and executing shell commands.

The agent implements an **agent loop**, allowing the model to reason step-by-step and execute tools until the final result is produced.

---

## Features

- **Agent Loop**
  - Enables multi-step reasoning and execution.

- **Tool Calling Support**
  - The model can dynamically call tools to perform actions.

- **File System Interaction**
  - Read files from disk
  - Write or modify files

- **Shell Command Execution**
  - Run bash commands directly from the agent.

- **LLM Integration**
  - Uses **OpenRouter API** for model inference.

- **CLI Interface**
  - Simple command-line interaction.

---

## Supported Tools

### Read Tool

Reads the contents of a file.

Example tool call:

```text
Read("README.md")
```

Use cases:

- Inspect project files
- Analyze documentation
- Read source code

---

### Write Tool

Writes content to a file.

Example tool call:

```text
Write("app/main.js", "console.log('Hello World')")
```

Behavior:

- Creates the file if it doesn't exist
- Overwrites the file if it already exists

---

### Bash Tool

Executes shell commands.

Example:

```text
Bash("rm README_old.md")
```

Use cases:

- Delete files
- Create directories
- Run scripts
- Manage project structure

---

## Architecture

The agent follows a **tool-calling loop architecture**:

```text
User Prompt
     │
     ▼
Send prompt to LLM
     │
     ▼
LLM Response
     │
     ├── Tool Call → Execute Tool → Return Result
     │
     ▼
Repeat until no tools requested
     │
     ▼
Print Final Answer
```

This is the same architecture used by modern AI agents such as:

- Cursor
- Claude Code
- OpenInterpreter
- Devin-style agents

---

## Project Structure

```
.
├── main.cpp
├── README.md
└── your_program.sh
```

---

## Requirements

- C++17 or later
- OpenRouter API key

Libraries used:

- `cpr`
- `nlohmann/json`

---

## Setup

### 1. Install dependencies

Install CPR and JSON libraries.

Example (Linux):

```bash
sudo apt install libcpr-dev
```

You may also install dependencies via your preferred package manager or build from source.

---

### 2. Set environment variables

```bash
export OPENROUTER_API_KEY=your_api_key
export OPENROUTER_BASE_URL=https://openrouter.ai/api/v1
```

---

### 3. Compile

Example:

```bash
g++ main.cpp -lcpr -o agent
```

---

## Usage

Run the agent from the terminal:

```bash
./agent -p "Read README.md and summarize it"
```

Example:

```bash
./agent -p "Delete README_old.md"
```

Example:

```bash
./agent -p "Read README.md and create a main.js file"
```

---

## Example Workflow

User prompt:

```text
Delete the old readme file. Always respond with `Deleted README_old.md`
```

Agent execution:

```text
LLM → Bash("rm README_old.md")
Program executes command
LLM → Deleted README_old.md
```

Output:

```text
Deleted README_old.md
```

---

## Key Concepts Demonstrated

This project demonstrates several important AI engineering concepts:

- LLM tool calling
- Autonomous agents
- Agent loop architecture
- File system automation
- Shell command execution through AI
- C++ integration with LLM APIs

---

## Future Improvements

Possible extensions:

- Add **Edit tool** for modifying files
- Add **Directory listing tool**
- Implement **tool registry architecture**
- Add **streaming responses**
- Add **multi-tool planning**
- Build a **code-editing AI assistant**

---

## License

MIT License
