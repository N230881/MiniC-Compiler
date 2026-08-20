# Compiler Construction Guide — MiniC Compiler + Streamlit UI

This is the complete reference for the project: file structure, what every
extension means, and the exact commands to build and run it on your laptop.

> **Update:** this project now matches the "IntelliCode Compiler Handbook"
> structure — it has a full pipeline of Lexer → Parser → AST → Semantic
> Analyzer → IR → **Optimizer** (constant folding + dead code elimination)
> → **Control Flow Graph** → Executor. See **Section 6** for exactly what
> changed and in which files.

---

## 1. Complete File Structure

```
minic-compiler-project/
│
├── token.h            # Module 1 (data): defines what a "token" looks like
├── lexer.h            # Module 1 (interface): function signatures for the lexer
├── lexer.c            # Module 1 (logic): converts raw text into tokens
│
├── ast.h              # Shared data structure: defines a "Node" (tree piece)
├── ast.c              # Helper functions to build/print the syntax tree
│
├── parser.h           # Module 2 (interface)
├── parser.c           # Module 2 (logic): tokens -> Abstract Syntax Tree (AST)
│
├── semantic.h          # Module 3 (interface)
├── semantic.c          # Module 3 (logic): symbol table + meaning checks
│
├── codegen.h           # Module 4 (interface): intermediate instruction format
├── codegen.c           # Module 4 (logic): AST -> Intermediate Representation (IR)
│
├── ops.h               # NEW: shared operator-evaluation logic
├── ops.c               # NEW: (used by both the Optimizer and the VM)
│
├── optimizer.h         # NEW Module 5 (interface)
├── optimizer.c         # NEW Module 5 (logic): constant folding + dead code elimination
│
├── cfg.h               # NEW: Control Flow Graph (interface)
├── cfg.c               # NEW: builds basic blocks + prints the CFG
│
├── vm.h                # Execution engine (interface)
├── vm.c                # Execution engine (logic): actually runs the optimized IR
│
├── main.c              # Orchestrator: wires every module together in order
│
├── minicompiler         # (generated) the compiled executable — not source
│
├── tests/
│   ├── test1.mc          # Sample program: variables, if/else, while, for
│   └── test2_optimizer.mc # Sample program that demonstrates optimization
│
├── app.py               # Streamlit web interface (calls the executable)
├── requirements.txt      # Python dependency list (just "streamlit")
│
└── Compiler_Construction_Guide.md   # this file
```

---

## 2. What every file extension means

| Extension | Meaning | Who uses it |
|---|---|---|
| `.h` | **Header file** — a "table of contents" that declares what functions/structs exist in a `.c` file, without the actual code. Other files `#include` it to use those functions. | C compiler (GCC) |
| `.c` | **C source file** — contains the actual logic/implementation. | C compiler (GCC) |
| `.mc` | **"MiniC"** — a file extension *we invented* for programs written in *your custom language* (not real C). This is the input your compiler reads, e.g. `tests/test1.mc`. | Your compiler (`minicompiler`) reads this |
| *(no extension)* `minicompiler` | The **compiled executable/binary** — the actual runnable program, produced by GCC from all the `.c` files. This is your "compiler.exe" equivalent. | Your operating system runs this |
| `.py` | **Python source file** — here, `app.py` is the Streamlit web UI that wraps your C executable. | Python interpreter |
| `.txt` | Plain text — `requirements.txt` just lists Python packages needed. | `pip` |

**Key distinction to remember:**
- `.c` / `.h` files = the compiler's own source code (written in real C, compiled once with GCC).
- `.mc` files = programs written *in the language your compiler understands* (the input it processes every time you run it).
- `minicompiler` = the finished translator program itself.

---

## 3. Complete Execution Process (Step by Step)

### Step A — Install prerequisites (one-time setup)

**On Windows:**
1. Install **WSL** (Windows Subsystem for Linux) — open PowerShell as admin and run:
   ```
   wsl --install
   ```
   Restart, then open "Ubuntu" from the Start menu. All commands below run inside that Ubuntu terminal.
2. Or, alternatively, install **MinGW-w64** and add it to PATH, then use regular Command Prompt.

**On Mac:**
```bash
xcode-select --install     # installs gcc/clang
```

**On Linux (Ubuntu/Debian):**
```bash
sudo apt update
sudo apt install gcc python3 python3-pip -y
```

Check installation:
```bash
gcc --version
python3 --version
```

### Step B — Get the project onto your laptop

Unzip the project folder you downloaded (`minic-compiler-project.zip`) anywhere, e.g. your Desktop, then open a terminal **inside that folder**:
```bash
cd path/to/minic-compiler-project
ls
```
You should see all the `.c`, `.h`, `app.py` files listed.

### Step C — Compile the compiler itself (build the executable)

This step turns your `.c` source files into one runnable program called `minicompiler`:
```bash
gcc -Wall -o minicompiler *.c
```
- `-Wall` → show all warnings (good practice)
- `-o minicompiler` → name the output file `minicompiler`
- the rest → all the `.c` source files being combined

If it finishes with no errors, you now have a file named `minicompiler` (or `minicompiler.exe` on Windows/MinGW) in that folder.

### Step D — Run your compiler on a test program

```bash
./minicompiler tests/test1.mc
```
On Windows Command Prompt (MinGW, not WSL):
```
minicompiler.exe tests\test1.mc
```

You will see 5 sections printed:
```
===TOKENS===       <- Module 1 output (Lexer)
===AST===          <- Module 2 output (Parser)
===SEMANTIC===      <- Module 3 output (Semantic Analyzer)
===IR===            <- Module 4 output (Code Generator: Intermediate Representation)
===OPTIMIZER===      <- Module 5 report (constant folding + dead code elimination)
===OPTIMIZED_IR===   <- IR after optimization
===CFG===            <- Control Flow Graph (basic blocks + branches)
===OUTPUT===         <- the program actually executing
```

### Step E — Write your own `.mc` program and test it

Create a new file `tests/mytest.mc`:
```c
int a = 10;
int b = 20;
print(a + b);
```
Run it:
```bash
./minicompiler tests/mytest.mc
```
Expected output at the bottom: `30`

### Step F — Set up and run the Streamlit interface

1. Install Streamlit (one-time):
   ```bash
   pip install streamlit
   ```
   (If `pip` fails, try `pip3 install streamlit` or `python3 -m pip install streamlit`)

2. Make sure `minicompiler` has already been built (Step C) and sits in the **same folder** as `app.py`.

3. Launch the web app:
   ```bash
   streamlit run app.py
   ```

4. Your browser will open automatically at `http://localhost:8501`. If not, open that link manually.

5. In the web page:
   - Edit or paste MiniC code in the text box
   - Click **"Compile & Run"**
   - See tabs for **Output**, **Tokens**, **AST**, **Semantic**, **IR**, **Optimizer**, **Optimized IR**, **CFG** — each showing that module's real result

### Step G — Rebuild after editing C code

Every time you change a `.c` or `.h` file, you must **recompile** before the changes take effect (Streamlit only calls the already-built executable, it doesn't rebuild it for you):
```bash
gcc -Wall -o minicompiler *.c
```
Then re-run `./minicompiler tests/test1.mc` or refresh the Streamlit page.

### Step H — Push to GitHub

```bash
git init
git add .
git commit -m "MiniC compiler with 4 modules + Streamlit UI"
git branch -M main
git remote add origin https://github.com/<your-username>/minic-compiler.git
git push -u origin main
```
Add a `.gitignore` with this content so you don't upload the compiled binary/junk:
```
minicompiler
minicompiler.exe
__pycache__/
*.pyc
```

---

## 4. Quick command cheat-sheet (copy-paste block)

```bash
# 1. Build
gcc -Wall -o minicompiler *.c

# 2. Run directly from terminal
./minicompiler tests/test1.mc

# 3. Install + run the web interface
pip install streamlit
streamlit run app.py
```

---

## 6. Changes made to match the IntelliCode Compiler Handbook

The handbook asks for a full pipeline (Lexer → Parser → AST → Semantic →
IR → **Optimizer** → Code Gen) instead of just Lexer+Parser+AST, plus
specific DSA concepts (Graphs/CFG) and specific optimizations (constant
folding, dead code elimination). Here's exactly what was added or changed,
file by file:

| Requirement from handbook | File(s) added/changed | What it does |
|---|---|---|
| "Intermediate Representation (IR)" | `codegen.c` / `codegen.h` (renamed output section) | The Three-Address Code output is now explicitly labeled `===IR===` in the pipeline, matching handbook terminology |
| "Optimizer" + "Constant Folding" | **`optimizer.c` / `optimizer.h`** (new files) | Scans the IR; whenever both sides of an operation are known constants (e.g. `2 + 3 * 4`), it computes the result at compile time and replaces it with one instruction |
| "Optimizer" + "Dead Code Elimination" | **`optimizer.c` / `optimizer.h`** (same new files) | Removes any instruction whose computed value is never actually used later, repeating until no more can be removed |
| "Graphs (Control Flow Graph)" | **`cfg.c` / `cfg.h`** (new files) | Splits the optimized IR into basic blocks using the standard "leaders" algorithm and prints each block with its outgoing edges (branch targets) |
| Clean architecture / no duplicated logic | **`ops.c` / `ops.h`** (new files) | Operator evaluation (`+ - * / % < > == && \|\|` etc.) used to be duplicated inside `vm.c`. It's now one shared module used by both the VM and the Optimizer — `vm.c` was edited to call `applyBinOp()`/`applyUnOp()` from `ops.h` instead |
| "Detailed Diagnostics" | already present in `lexer.c`, `parser.c`, `semantic.c` | Every error already reports the exact line number and what was expected — no change needed, this was already implemented |
| Pipeline orchestration | **`main.c`** (updated) | Now runs: Tokens → AST → Semantic → IR → Optimizer report → Optimized IR → CFG → Output, printing a labeled section for each stage |
| Web IDE showing every stage | **`app.py`** (updated) | Added 3 new tabs: "Optimizer" (shows what was folded/eliminated), "Optimized IR", and "CFG" |

### How to see the optimizer in action

Run the new test file that's designed to demonstrate both optimizations:
```bash
./minicompiler tests/test2_optimizer.mc
```
Look at the `===OPTIMIZER===` section — it will report something like:
```
Constant folding      : simplified 3 expression(s)
Dead code elimination : removed 6 unused instruction(s)
```
Then compare `===IR===` (before) against `===OPTIMIZED_IR===` (after) — you'll
see `2 + 3 * 4` collapsed into a single `t4 = 14` line, and the now-useless
intermediate temp variables gone entirely.

### Handbook items intentionally not implemented (and why)

- **Full LLVM/GCC-style optimization suite** (loop unrolling, inlining, register allocation, SSA form) — these are multi-month engineering efforts in real compilers; the handbook's own comparison table only asks for *constant folding + dead code elimination*, which is implemented.
- **CI/CD pipeline** — this depends on your GitHub repo setup, not the compiler code itself. Once you push to GitHub (Section 3, Step H), I can help you add a GitHub Actions workflow file (`.github/workflows/build.yml`) that auto-compiles and runs the test suite on every push, if you want that next.
- **Unit testing framework** — currently tested via the `tests/*.mc` sample files. If you want a proper pass/fail test harness (e.g. asserting exact output per file), say so and it can be added as `tests/run_tests.sh`.

---

## 7. What language your compiler supports (MiniC)

| Feature | Example |
|---|---|
| Variable declaration | `int x = 5;` |
| Arithmetic | `+  -  *  /  %` |
| Comparison | `<  >  <=  >=  ==  !=` |
| Logical | `&&  \|\|  !` |
| Conditionals | `if (x > 0) { ... } else { ... }` |
| Loops | `while (...) { ... }`  and  `for (int i=0; i<n; i=i+1) { ... }` |
| Output | `print(expr);` |
| Blocks | `{ statement; statement; }` |

This covers the core control-flow and expression handling that real C programs use, which is exactly what a college-level "Compiler Construction" project is graded on. It does not implement the entire real C standard (pointers, structs, arrays, functions, the C standard library) — building that is what production compilers like GCC/Clang do, representing years of engineering. If your project rubric explicitly requires one or two of these (e.g. functions or arrays), tell me which and I'll add that module next.
