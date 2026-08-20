import streamlit as st
import subprocess
import tempfile
import os
import stat
st.set_page_config(page_title="MiniC Compiler", layout="wide")
COMPILER_PATH = os.path.join(os.path.dirname(__file__), "minicompiler")
if os.path.exists(COMPILER_PATH):
    os.chmod(COMPILER_PATH, stat.S_IRWXU)
st.title("🛠️ MiniC Compiler — Web Interface")
st.caption("A custom C-like language compiled through 4 real stages: Lexer → Parser → Semantic Analyzer → Code Generator, then executed.")

DEFAULT_CODE = """int a = 2 + 3 * 4;   // constant folding will pre-compute this to 14
int unused = 100 + 200;  // dead code elimination will remove this work
int b = 10;
print(a);
print(b);

if (a > 5) {
    print(1);
} else {
    print(0);
}

int i = 0;
int sum = 0;
while (i < 5) {
    sum = sum + i;
    i = i + 1;
}
print(sum);
"""

# Path to the compiled C executable (built with gcc, see README)
COMPILER_PATH = os.path.join(os.path.dirname(__file__), "minicompiler")

code = st.text_area("Write your MiniC program here:", value=DEFAULT_CODE, height=300)

run_btn = st.button("▶️ Compile & Run", type="primary")

def parse_sections(raw_output: str) -> dict:
    """Split the compiler's stdout into sections using the ===MARKER=== lines."""
    sections = {}
    current = None
    buf = []
    for line in raw_output.splitlines():
        if line.startswith("===") and line.endswith("==="):
            if current:
                sections[current] = "\n".join(buf)
            current = line.strip("=")
            buf = []
        else:
            buf.append(line)
    if current:
        sections[current] = "\n".join(buf)
    return sections

if run_btn:
    if not os.path.exists(COMPILER_PATH):
        st.error(
            f"Compiler executable not found at `{COMPILER_PATH}`.\n\n"
            "Build it first by running this in a terminal, inside this folder:\n\n"
            "```\ngcc -Wall -o minicompiler main.c lexer.c ast.c parser.c semantic.c codegen.c vm.c\n```"
        )
    else:
        with tempfile.NamedTemporaryFile(mode="w", suffix=".mc", delete=False) as f:
            f.write(code)
            src_path = f.name

        try:
            result = subprocess.run(
                [COMPILER_PATH, src_path],
                capture_output=True,
                text=True,
                timeout=5,
            )
            sections = parse_sections(result.stdout)

            if result.returncode != 0 or result.stderr:
                st.error("Compiler reported an error:")
                st.code(result.stderr or "Unknown error", language="text")

            tabs = st.tabs([
                "✅ Output", "🔤 Tokens", "🌳 AST", "🧠 Semantic",
                "⚙️ IR", "🚀 Optimizer", "📉 Optimized IR", "🕸️ CFG",
            ])

            with tabs[0]:
                st.subheader("Program Output")
                st.code(sections.get("OUTPUT", "(no output)"), language="text")

            with tabs[1]:
                st.subheader("Module 1: Lexical Analyzer — Tokens")
                st.code(sections.get("TOKENS", ""), language="text")

            with tabs[2]:
                st.subheader("Module 2: Syntax Analyzer — Abstract Syntax Tree")
                st.code(sections.get("AST", ""), language="text")

            with tabs[3]:
                st.subheader("Module 3: Semantic Analyzer")
                st.code(sections.get("SEMANTIC", ""), language="text")

            with tabs[4]:
                st.subheader("Module 4: Code Generator — Intermediate Representation (IR)")
                st.code(sections.get("IR", ""), language="text")

            with tabs[5]:
                st.subheader("Module 5: Optimizer — Report")
                st.code(sections.get("OPTIMIZER", ""), language="text")
                st.caption("Constant folding pre-computes constant expressions (e.g. 2+3*4 → 14). "
                           "Dead code elimination removes instructions whose result is never used.")

            with tabs[6]:
                st.subheader("IR After Optimization")
                st.code(sections.get("OPTIMIZED_IR", ""), language="text")

            with tabs[7]:
                st.subheader("Control Flow Graph (Basic Blocks + Edges)")
                st.code(sections.get("CFG", ""), language="text")

        except subprocess.TimeoutExpired:
            st.error("Program took too long to run (possible infinite loop).")
        finally:
            os.unlink(src_path)

st.divider()
st.caption("MiniC supports: int declarations, arithmetic (+ - * / %), comparisons, && || !, if/else, while, for, print(), and { } blocks.")
