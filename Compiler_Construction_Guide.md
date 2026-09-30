# 📘 MiniC Compiler

## Compiler Construction Guide

### A Technical Guide to Architecture, Implementation, Optimization & Execution

> **MiniC is a custom C-like compiler built from scratch in C, with an interactive Streamlit Compiler Explorer.**
>
> This handbook explains the complete project architecture, every source module, file extension, compilation stage, optimization pass, Control Flow Graph construction, execution model, build process, testing workflow, and Streamlit interface.

---

# 🧭 Guide at a Glance

| Area                            | Implementation                           |
| ------------------------------- | ---------------------------------------- |
| **Language**                    | MiniC — custom C-like language           |
| **Compiler Language**           | C                                        |
| **Frontend**                    | Lexer → Parser → AST                     |
| **Semantic Processing**         | Symbol table + semantic checks           |
| **Intermediate Representation** | Three-Address Code / IR                  |
| **Optimization**                | Constant Folding + Dead Code Elimination |
| **Graph Analysis**              | Control Flow Graph                       |
| **Execution**                   | Custom Virtual Machine / Executor        |
| **Web Interface**               | Streamlit                                |
| **Testing**                     | `.mc` test programs                      |
| **Build System**                | GCC                                      |
| **Platform Support**            | Linux / macOS / Windows via WSL or MinGW |

---

# 1. 🎯 Project Overview

MiniC is a **complete educational compiler pipeline** designed to demonstrate the major concepts involved in compiler construction.

The project begins with a MiniC source program and progressively transforms it through multiple representations until the program can be executed.

```text
                 MINI C SOURCE
                      │
                      ▼
              ┌───────────────┐
              │     LEXER     │
              │ Source →      │
              │ Tokens        │
              └───────┬───────┘
                      │
                      ▼
              ┌───────────────┐
              │    PARSER     │
              │ Tokens → AST  │
              └───────┬───────┘
                      │
                      ▼
              ┌───────────────┐
              │  SEMANTIC     │
              │   ANALYZER    │
              │ Meaning       │
              │ Checks        │
              └───────┬───────┘
                      │
                      ▼
              ┌───────────────┐
              │      IR       │
              │ AST → TAC     │
              └───────┬───────┘
                      │
                      ▼
              ┌───────────────┐
              │   OPTIMIZER   │
              │ Constant      │
              │ Folding + DCE │
              └───────┬───────┘
                      │
                      ▼
              ┌───────────────┐
              │      CFG      │
              │ Basic Blocks  │
              │ + Branches    │
              └───────┬───────┘
                      │
                      ▼
              ┌───────────────┐
              │   EXECUTOR    │
              │ Optimized IR  │
              └───────┬───────┘
                      │
                      ▼
                  PROGRAM
                   OUTPUT
```

The current implementation follows the expanded pipeline:

**Lexer → Parser → AST → Semantic Analyzer → IR → Optimizer → Control Flow Graph → Executor**.

---

# 2. 🏗️ Compiler Architecture

The architecture is intentionally modular.

Each compiler phase has a specific responsibility and communicates with the next phase through a well-defined representation.

```text
┌─────────────────────────────────────────────────────────────┐
│                      MiniC Source Code                      │
│                           (.mc)                             │
└─────────────────────────────┬───────────────────────────────┘
                              │
                              ▼
                    ┌──────────────────┐
                    │      LEXER       │
                    │                  │
                    │ Source → Tokens  │
                    └────────┬─────────┘
                             │
                             ▼
                    ┌──────────────────┐
                    │      PARSER      │
                    │                  │
                    │ Tokens → AST     │
                    └────────┬─────────┘
                             │
                             ▼
                    ┌──────────────────┐
                    │     SEMANTIC     │
                    │     ANALYZER     │
                    │                  │
                    │ Symbol + Meaning │
                    └────────┬─────────┘
                             │
                             ▼
                    ┌──────────────────┐
                    │       IR         │
                    │                  │
                    │ AST → TAC        │
                    └────────┬─────────┘
                             │
                             ▼
                    ┌──────────────────┐
                    │    OPTIMIZER     │
                    │                  │
                    │ Folding + DCE    │
                    └────────┬─────────┘
                             │
                             ▼
                    ┌──────────────────┐
                    │       CFG        │
                    │                  │
                    │ Blocks + Edges   │
                    └────────┬─────────┘
                             │
                             ▼
                    ┌──────────────────┐
                    │    EXECUTOR      │
                    │                  │
                    │ Optimized IR     │
                    └──────────────────┘
```

---

# 3. 📁 Complete Project Structure

```text
minic-compiler-project/
│
├── token.h
├── lexer.h
├── lexer.c
│
├── ast.h
├── ast.c
│
├── parser.h
├── parser.c
│
├── semantic.h
├── semantic.c
│
├── codegen.h
├── codegen.c
│
├── ops.h
├── ops.c
│
├── optimizer.h
├── optimizer.c
│
├── cfg.h
├── cfg.c
│
├── vm.h
├── vm.c
│
├── main.c
│
├── minicompiler
│
├── tests/
│   ├── test1.mc
│   └── test2_optimizer.mc
│
├── app.py
├── requirements.txt
│
└── Compiler_Construction_Guide.md
```

The project structure separates **compiler data structures, interfaces, implementations, optimization, graph analysis, execution, testing, and UI** into distinct components.

---

# 4. 🧩 Module-by-Module Architecture

## 4.1 Token Module

### `token.h`

Defines what a token looks like.

```text
token.h
   │
   └── Token data structures
```

It provides the shared token representation consumed by the lexer and parser.

---

# 4.2 🔤 Lexer Module

### `lexer.h`

Defines the public lexer interface and function signatures.

### `lexer.c`

Contains the actual lexical-analysis logic.

Its responsibility is:

```text
Raw MiniC Source
       │
       ▼
     Lexer
       │
       ▼
    Tokens
```

The lexer converts raw characters into meaningful lexical units such as:

* Keywords
* Identifiers
* Integer literals
* Operators
* Punctuation

---

# 4.3 🌳 AST Module

### `ast.h`

Defines the AST node structure.

A node represents one piece of the syntax tree.

### `ast.c`

Contains helper functions used to:

* Create AST nodes
* Manipulate AST nodes
* Print the syntax tree

The AST becomes the structured representation consumed by later compiler phases.

---

# 4.4 🧠 Parser Module

### `parser.h`

Defines the parser interface.

### `parser.c`

Converts:

```text
Tokens
   ↓
Abstract Syntax Tree
```

The parser is responsible for validating the syntactic structure of the program and constructing the corresponding AST.

---

# 4.5 🔎 Semantic Analysis Module

### `semantic.h`

Defines the semantic-analysis interface.

### `semantic.c`

Implements:

* Symbol-table management
* Identifier checks
* Meaning checks
* Semantic diagnostics

Conceptually:

```text
AST
 │
 ▼
Semantic Analyzer
 │
 ├── Symbol checks
 ├── Meaning checks
 └── Diagnostics
 │
 ▼
Validated Program
```

---

# 4.6 ⚙️ Intermediate Representation Module

### `codegen.h`

Defines the intermediate instruction format.

### `codegen.c`

Converts:

```text
AST
 │
 ▼
Three-Address Code / IR
```

The IR creates an intermediate layer between the high-level AST and the later optimization/execution stages.

---

# 4.7 🧮 Shared Operator Module

### `ops.h`

Defines shared operator-evaluation functionality.

### `ops.c`

Implements operator evaluation used by both:

```text
Optimizer
    │
    └── ops.c

Virtual Machine
    │
    └── ops.c
```

This avoids duplicating operator logic between the optimizer and VM.

Supported operator evaluation includes operators such as:

```text
+  -  *  /  %
<  >  ==  &&  ||
```

## The shared module was introduced specifically to improve architectural cleanliness and reuse.

# 4.8 🚀 Optimizer Module

### `optimizer.h`

Defines the optimizer interface.

### `optimizer.c`

Implements two optimization techniques:

### Constant Folding

Evaluates constant expressions during compilation.

### Dead Code Elimination

Removes instructions whose computed values are never used.

The optimizer repeatedly removes dead instructions until no further elimination is possible.

---

# 4.9 🕸️ Control Flow Graph Module

### `cfg.h`

Defines the CFG interface.

### `cfg.c`

Builds and prints:

* Basic blocks
* Control-flow edges
* Branch relationships

## The CFG is constructed from optimized IR using a standard **leader-based basic-block algorithm**.

# 4.10 🖥️ Virtual Machine / Execution Module

### `vm.h`

Defines the execution-engine interface.

### `vm.c`

Actually executes the optimized IR.

```text
Optimized IR
     │
     ▼
┌───────────────┐
│ Virtual       │
│ Machine       │
└───────┬───────┘
        │
        ▼
Program Output
```

---

# 4.11 🎛️ Main Orchestrator

### `main.c`

`main.c` connects every compiler phase in the correct order.

The pipeline is:

```text
Tokens
  ↓
AST
  ↓
Semantic
  ↓
IR
  ↓
Optimizer
  ↓
Optimized IR
  ↓
CFG
  ↓
Output
```

## It also prints a labeled section for every stage.

# 5. 📄 Understanding File Extensions

| Extension    | Meaning                                                        | Used By            |
| ------------ | -------------------------------------------------------------- | ------------------ |
| `.h`         | Header file containing declarations, structures and interfaces | GCC                |
| `.c`         | C source file containing implementation                        | GCC                |
| `.mc`        | MiniC source program written in the custom language            | `minicompiler`     |
| `.py`        | Python source code                                             | Python / Streamlit |
| `.txt`       | Plain-text configuration/dependency file                       | `pip`              |
| No extension | Compiled `minicompiler` executable                             | Operating system   |

These distinctions are fundamental to understanding how the project is built and executed.

---

## ⭐ Key Distinction

```text
.c / .h
   │
   └── Source code of the compiler itself

.mc
   │
   └── Programs written in the MiniC language

minicompiler
   │
   └── Finished compiler executable
```

In other words:

> **GCC compiles the compiler. The MiniC compiler compiles MiniC programs.**

---

# 6. 🔄 Complete Execution Process

## Step A — Install Prerequisites

### 🪟 Windows

### Option 1 — WSL

Open PowerShell as Administrator:

```powershell
wsl --install
```

Restart the system and open **Ubuntu** from the Start menu.

The remaining Linux commands can then be executed inside Ubuntu.

### Option 2 — MinGW-w64

Alternatively:

1. Install MinGW-w64.
2. Add GCC to `PATH`.
3. Use Command Prompt or PowerShell.

---

## 🍎 macOS

Install the command-line developer tools:

```bash
xcode-select --install
```

---

## 🐧 Ubuntu / Debian

```bash
sudo apt update
sudo apt install gcc python3 python3-pip -y
```

---

## Verify Installation

```bash
gcc --version
python3 --version
```

These commands confirm that the required compiler and Python runtime are available.

---

# 7. 📦 Get the Project

If using the downloaded project archive:

```bash
cd path/to/minic-compiler-project
ls
```

You should see the `.c`, `.h`, `app.py`, and other project files.

The original workflow assumes the project has been downloaded/unzipped locally before building.

---

# 8. 🔨 Build the Compiler

The compiler itself must first be compiled.

Run:

```bash
gcc -Wall -o minicompiler *.c
```

### What the command means

```text
-Wall
 │
 └── Enables compiler warnings

-o minicompiler
 │
 └── Names the generated executable

*.c
 │
 └── Includes all C source files
```

After successful compilation:

```text
minicompiler
```

is created.

On Windows/MinGW:

```text
minicompiler.exe
```

is created.

---

# 9. ▶️ Run a MiniC Program

## Linux / macOS / WSL

```bash
./minicompiler tests/test1.mc
```

## Windows — MinGW

```text
minicompiler.exe tests\test1.mc
```

The compiler then exposes the results of each major phase.

---

# 10. 📊 Compiler Output

The execution output is organized into labeled sections:

```text
===TOKENS===
```

**Module 1 — Lexer**

```text
===AST===
```

**Module 2 — Parser**

```text
===SEMANTIC===
```

**Module 3 — Semantic Analyzer**

```text
===IR===
```

**Module 4 — Code Generator / Intermediate Representation**

```text
===OPTIMIZER===
```

**Module 5 — Optimization report**

```text
===OPTIMIZED_IR===
```

**IR after optimization**

```text
===CFG===
```

**Control Flow Graph — basic blocks and branches**

```text
===OUTPUT===
```

**Final program execution**

The complete output sequence is defined by the project pipeline.

---

# 11. 🧪 Write Your Own MiniC Program

Create:

```text
tests/mytest.mc
```

Example:

```c
int a = 10;
int b = 20;
print(a + b);
```

Run:

```bash
./minicompiler tests/mytest.mc
```

Expected final output:

```text
30
```

This provides a simple end-to-end test of the compiler pipeline.

---

# 12. 🌐 Streamlit Compiler Explorer

The project includes a **Streamlit-based web interface** that acts as an interactive compiler explorer.

### `app.py`

The web interface wraps the compiled C executable and exposes the compiler's internal stages in a browser.

```text
                   MiniC Code
                       │
                       ▼
                ┌──────────────┐
                │ Compile & Run│
                └──────┬───────┘
                       │
       ┌───────────────┼────────────────┐
       ▼               ▼                ▼
    Tokens            AST            Semantic
       │               │                │
       └───────────────┼────────────────┘
                       ▼
                      IR
                       │
             ┌─────────┴─────────┐
             ▼                   ▼
        Optimizer          Optimized IR
             │                   │
             └─────────┬─────────┘
                       ▼
                      CFG
                       │
                       ▼
                     Output
```

---

# 13. 🚀 Launch the Streamlit UI

## Install Streamlit

```bash
pip install streamlit
```

If `pip` does not work:

```bash
pip3 install streamlit
```

or:

```bash
python3 -m pip install streamlit
```

---

## Ensure the Compiler Exists

Before launching Streamlit, build:

```bash
gcc -Wall -o minicompiler *.c
```

The executable must be in the **same directory as `app.py`**.

---

## Start the Application

```bash
streamlit run app.py
```

The browser normally opens automatically at:

```text
http://localhost:8501
```

If it does not, open the address manually.

---

# 14. 🖥️ Using the Compiler Explorer

Inside the web interface:

1. Write or paste MiniC code.
2. Click **Compile & Run**.
3. Inspect the individual compiler stages.

Available views include:

```text
┌───────────────────────┐
│        Output         │
├───────────────────────┤
│        Tokens         │
├───────────────────────┤
│         AST           │
├───────────────────────┤
│      Semantic         │
├───────────────────────┤
│          IR           │
├───────────────────────┤
│      Optimizer        │
├───────────────────────┤
│    Optimized IR       │
├───────────────────────┤
│         CFG           │
└───────────────────────┘
```

Each view displays the **actual result produced by that compiler stage**, rather than a static illustration.

---

# 15. 🔁 Rebuild After Editing C Code

This is an important development rule.

Whenever you modify:

```text
*.c
*.h
```

you must rebuild the compiler.

```bash
gcc -Wall -o minicompiler *.c
```

Why?

Because Streamlit calls the **already-built executable**. It does not automatically compile the C source files.

### Correct development cycle

```text
Edit C Code
    ↓
Rebuild
    ↓
Run / Refresh
    ↓
Test
    ↓
Repeat
```

---

# 16. 🧠 Optimization Deep Dive

The optimizer is one of the major extensions of the compiler.

The implementation includes:

```text
┌──────────────────────────────┐
│          OPTIMIZER           │
├──────────────────────────────┤
│                              │
│  1. Constant Folding         │
│                              │
│  2. Dead Code Elimination    │
│                              │
└──────────────────────────────┘
```

---

# 17. Constant Folding

Constant folding evaluates expressions during compilation when all required values are known.

Consider:

```text
2 + 3 * 4
```

Instead of repeatedly performing the calculation during execution, the optimizer can evaluate it at compile time:

```text
2 + 3 * 4
     ↓
   14
```

The resulting IR can therefore contain:

```text
t4 = 14
```

rather than the entire intermediate calculation sequence.

The project specifically uses this optimization to simplify constant expressions in IR.

---

# 18. 🧹 Dead Code Elimination

Dead Code Elimination removes instructions whose computed values are never actually used later.

Conceptually:

```text
Before:

t1 = ...
t2 = ...
t3 = t1 + t2
unused = t3
```

If `unused` does not contribute to observable program behavior, the corresponding computation can be removed.

The optimizer repeats the elimination process until no additional dead instructions can be removed.

---

# 19. 🔬 Optimization Demonstration

The project includes:

```text
tests/test2_optimizer.mc
```

Run:

```bash
./minicompiler tests/test2_optimizer.mc
```

The optimizer report can contain results such as:

```text
Constant folding      : simplified 3 expression(s)
Dead code elimination : removed 6 unused instruction(s)
```

The exact numbers depend on the test program.

---

# 20. Before vs After Optimization

The most useful way to understand the optimizer is to compare:

```text
===IR===
```

against:

```text
===OPTIMIZED_IR===
```

For example:

### Before

```text
t0 = 2
t1 = 3
t2 = 4
t3 = t1 * t2
t4 = t0 + t3
```

### After

```text
t4 = 14
```

The intermediate temporary calculations have been collapsed through constant folding.

The optimizer can also eliminate temporary instructions that become unnecessary after the transformation.

---

# 21. 🕸️ Control Flow Graph

The compiler also implements Control Flow Graph construction.

A CFG represents the flow of execution through a program.

```text
                    ┌───────────┐
                    │  Block 0  │
                    └─────┬─────┘
                          │
                    condition
                     /         \
                    ▼           ▼
             ┌──────────┐  ┌──────────┐
             │ Block 1  │  │ Block 2  │
             └────┬─────┘  └────┬─────┘
                  │              │
                  └──────┬───────┘
                         ▼
                  ┌───────────┐
                  │  Block 3  │
                  └───────────┘
```

The implementation:

1. Identifies **leaders**.
2. Creates **basic blocks**.
3. Determines control-flow relationships.
4. Prints blocks and outgoing edges.

This uses the standard leader-based approach described in the project requirements.

---

# 22. 🧮 Shared Operator Evaluation

Operator evaluation was separated into:

```text
ops.h
ops.c
```

Previously, operator behavior was duplicated inside the VM.

The architecture now centralizes it:

```text
                 ┌──────────────┐
                 │   ops.c      │
                 │              │
                 │ applyBinOp() │
                 │ applyUnOp()  │
                 └──────┬───────┘
                        │
              ┌─────────┴─────────┐
              ▼                   ▼
          Optimizer               VM
```

This improves:

* Code reuse
* Maintainability
* Consistency
* Separation of concerns

The VM was updated to use the shared operator functions from `ops.h`.

---

# 23. 🛡️ Diagnostics

Detailed diagnostics were already present in the project.

The following modules report errors with source-line information:

```text
lexer.c
parser.c
semantic.c
```

The diagnostics communicate:

* Where the problem occurred
* Which line contains the error
* What was expected

No additional architectural change was required for this functionality.

---

# 24. 🧩 Pipeline Orchestration

`main.c` is responsible for coordinating the entire compiler.

The updated pipeline is:

```text
Tokens
  ↓
AST
  ↓
Semantic Analysis
  ↓
IR
  ↓
Optimizer Report
  ↓
Optimized IR
  ↓
CFG
  ↓
Program Output
```

Every stage is printed with a dedicated label, making the compiler's internal transformation process easy to inspect.

---

# 25. 🌐 Web IDE Enhancements

The Streamlit interface was expanded to expose three additional stages:

```text
Optimizer
Optimized IR
CFG
```

This means the web interface now visualizes not only the frontend but also the compiler's optimization and control-flow stages.

---

# 26. 📚 MiniC Language Specification

MiniC supports the following core language constructs.

| Feature                  | Example                             |
| ------------------------ | ----------------------------------- |
| **Variable declaration** | `int x = 5;`                        |
| **Arithmetic**           | `+  -  *  /  %`                     |
| **Comparison**           | `<  >  <=  >=  ==  !=`              |
| **Logical**              | `&&  \|\|  !`                       |
| **Conditionals**         | `if (x > 0) { ... } else { ... }`   |
| **While loop**           | `while (...) { ... }`               |
| **For loop**             | `for (int i=0; i<n; i=i+1) { ... }` |
| **Output**               | `print(expr);`                      |
| **Blocks**               | `{ statement; statement; }`         |

These constructs cover the core control-flow and expression handling supported by the current compiler.

---

# 27. 🧪 Example MiniC Program

```c
int x = 5;
int y = 10;

if (x < y) {
    print(x);
} else {
    print(y);
}
```

The compiler processes this through:

```text
Source
  ↓
Tokens
  ↓
AST
  ↓
Semantic Checks
  ↓
IR
  ↓
Optimization
  ↓
CFG
  ↓
Execution
```

This illustrates why the project is more than a lexer/parser demonstration: the same source program is transformed through multiple compiler representations.

---

# 28. 🔬 Exact Project Changes

The project was extended to match the **IntelliCode Compiler Handbook** requirements.

| Requirement                 | File(s)                             | Implementation                                        |
| --------------------------- | ----------------------------------- | ----------------------------------------------------- |
| Intermediate Representation | `codegen.c`, `codegen.h`            | Three-Address Code is explicitly labeled `===IR===`   |
| Constant Folding            | `optimizer.c`, `optimizer.h`        | Compile-time evaluation of known constant expressions |
| Dead Code Elimination       | `optimizer.c`, `optimizer.h`        | Removes unused instructions                           |
| Control Flow Graph          | `cfg.c`, `cfg.h`                    | Builds basic blocks and outgoing edges                |
| Shared Operator Logic       | `ops.c`, `ops.h`                    | Centralizes operator evaluation                       |
| Detailed Diagnostics        | `lexer.c`, `parser.c`, `semantic.c` | Line-aware error reporting                            |
| Pipeline Orchestration      | `main.c`                            | Runs all compiler stages sequentially                 |
| Web IDE                     | `app.py`                            | Adds Optimizer, Optimized IR and CFG views            |

This mapping preserves the project's original implementation notes while presenting them in a more structured engineering format.

---

# 29. 🚧 Intentionally Unimplemented Features

The project intentionally does **not** attempt to reproduce the complete optimization infrastructure of production compilers.

## Full LLVM/GCC-style optimization suite

Not implemented:

* Loop unrolling
* Function inlining
* Register allocation
* SSA form

These are significantly larger engineering problems. The current handbook scope focuses on:

```text
Constant Folding
        +
Dead Code Elimination
```

which are implemented in the project.

---

## CI/CD Pipeline

A CI/CD pipeline is not part of the current compiler implementation.

Once the repository is hosted on GitHub, a GitHub Actions workflow can be added to:

```text
Push
  ↓
Build
  ↓
Run Tests
  ↓
Report Result
```

The original project notes identify this as a repository/automation concern rather than compiler functionality.

---

## Unit Testing Framework

The current project uses:

```text
tests/
├── test1.mc
└── test2_optimizer.mc
```

as sample-based testing.

A future test harness could execute each `.mc` file and verify its expected output automatically.

For example:

```text
test1.mc
   ↓
Compile
   ↓
Execute
   ↓
Compare Expected Output
   ↓
PASS / FAIL
```

The original documentation identifies this as a possible future `tests/run_tests.sh` extension.

---

# 30. 🔧 Git Workflow

Initialize the repository:

```bash
git init
```

Stage files:

```bash
git add .
```

Create the first commit:

```bash
git commit -m "MiniC compiler with 4 modules + Streamlit UI"
```

Set the main branch:

```bash
git branch -M main
```

Add the remote:

```bash
git remote add origin https://github.com/<your-username>/minic-compiler.git
```

Push:

```bash
git push -u origin main
```

These commands provide the basic workflow for publishing the compiler project to GitHub.

---

# 31. 🧹 Recommended `.gitignore`

The generated executable and Python cache files should not be committed as source files.

Create:

```text
.gitignore
```

with:

```gitignore
minicompiler
minicompiler.exe

__pycache__/
*.pyc
```

This keeps generated binaries and Python cache artifacts out of version control.

---

# 32. ⚡ Quick Command Cheat Sheet

## Build

```bash
gcc -Wall -o minicompiler *.c
```

## Run

```bash
./minicompiler tests/test1.mc
```

## Install Streamlit

```bash
pip install streamlit
```

## Launch Web UI

```bash
streamlit run app.py
```

## Run Optimization Test

```bash
./minicompiler tests/test2_optimizer.mc
```

### Complete workflow

```text
┌──────────────┐
│ Edit Source  │
└──────┬───────┘
       ▼
┌──────────────┐
│     Build    │
│     GCC      │
└──────┬───────┘
       ▼
┌──────────────┐
│  Run MiniC   │
└──────┬───────┘
       ▼
┌──────────────────────────┐
│ Tokens → AST → Semantic  │
│ → IR → Optimize → CFG    │
│ → Execute                │
└──────────────────────────┘
```

The original guide provides the same core command sequence as its quick-reference workflow.

---

# 33. 🧠 Compiler Concepts Demonstrated

This project provides hands-on implementation of several important compiler-construction concepts:

### Frontend

* Lexical analysis
* Tokenization
* Parsing
* Abstract Syntax Trees
* Semantic analysis
* Symbol tables
* Diagnostics

### Intermediate Representation

* Three-Address Code
* Temporary variables
* Instruction-level representation

### Optimization

* Constant folding
* Dead code elimination

### Program Analysis

* Basic blocks
* Control Flow Graphs
* Branch relationships

### Backend / Execution

* Intermediate instruction execution
* Virtual-machine style execution
* Shared operator evaluation

### Developer Tooling

* GCC build workflow
* Command-line execution
* Streamlit compiler explorer
* Git/GitHub workflow

---

# 34. 🧱 Why the Architecture Is Modular

The compiler deliberately separates responsibilities.

```text
Lexer
  │
  └── Only understands characters/tokens

Parser
  │
  └── Only builds syntactic structure

Semantic Analyzer
  │
  └── Validates meaning

Code Generator
  │
  └── Produces IR

Optimizer
  │
  └── Improves IR

CFG
  │
  └── Analyzes control flow

VM
  │
  └── Executes IR
```

This modularity makes it possible to improve one phase without rewriting the entire compiler.

For example:

```text
New Optimization
      │
      ▼
optimizer.c
      │
      ▼
No need to rewrite
Lexer / Parser / AST
```

Likewise, a future CFG visualization enhancement can be implemented within the CFG/UI layers without redesigning lexical analysis.

---

# 35. 🔭 Future Engineering Roadmap

The current architecture creates natural extension points for future work.

```text
Current Compiler
       │
       ├── Constant Folding
       ├── Dead Code Elimination
       └── CFG
             │
             ▼
     Future Extensions
             │
             ├── Constant Propagation
             ├── Copy Propagation
             ├── Common Subexpression Elimination
             ├── Strength Reduction
             ├── Loop Optimizations
             ├── Function Support
             ├── Arrays
             ├── Automated Test Harness
             ├── CI/CD
             └── Advanced CFG Visualization
```

These extensions can be introduced incrementally because the compiler already has a modular intermediate-representation and analysis pipeline.

---

# 36. 🏆 Engineering Summary

MiniC demonstrates the transformation of a custom programming language through a complete compiler pipeline:

```text
                 SOURCE
                   │
                   ▼
                LEXING
                   │
                   ▼
                PARSING
                   │
                   ▼
                  AST
                   │
                   ▼
          SEMANTIC ANALYSIS
                   │
                   ▼
                  IR
                   │
                   ▼
              OPTIMIZATION
             ╱             ╲
   Constant Folding    Dead Code Elimination
             ╲             ╱
                   ▼
                  CFG
                   │
                   ▼
               EXECUTION
                   │
                   ▼
                 OUTPUT
```

The project therefore demonstrates not only **how a compiler reads source code**, but also how source programs are represented, validated, transformed, optimized, analyzed, and ultimately executed.

---

# 📌 Final Reference

### Core compiler pipeline

```text
Lexer
  ↓
Parser
  ↓
AST
  ↓
Semantic Analyzer
  ↓
IR
  ↓
Optimizer
  ├── Constant Folding
  └── Dead Code Elimination
  ↓
Control Flow Graph
  ↓
Executor / Virtual Machine
  ↓
Program Output
```

### Build

```bash
gcc -Wall -o minicompiler *.c
```

### Execute

```bash
./minicompiler tests/test1.mc
```

### Launch Compiler Explorer

```bash
streamlit run app.py
```

### Optimization Demonstration

```bash
./minicompiler tests/test2_optimizer.mc
```

---

> ## 🎓 MiniC Compiler
>
> **From source text → tokens → syntax tree → semantic validation → intermediate representation → optimization → control-flow analysis → execution.**
>
> A compact implementation of the core ideas behind modern compiler construction.
