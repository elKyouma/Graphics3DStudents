# Using AI coding agents

AI coding agents (Claude Code, Codex, Copilot, Cursor, ...) can write a working solution to most of the assignments
in this course in a few minutes. That is exactly why you should be careful with them: the goal of the course is not
the code, it is that **you** understand how the graphics pipeline works. If the agent does the thinking, you get the
program but not the knowledge, and the exam and later projects will show it.

Used well, an agent is a patient tutor that is available at any time. Below are some rules that help to use it that way.

## Do

1. **Try first yourself.** Read the assignment, the lecture notes and the OpenGL documentation, and write your own
   attempt before asking for help. Struggling with a problem for a while is how you learn.
2. **Ask for explanations, not solutions.** For example:
    - "Explain what `glVertexArrayAttribFormat` does and what each of its arguments means."
    - "Why do I need a projection matrix? Do not write the code."
    - "My triangle is not visible. What are the possible causes? Give me hints, not a fix."
3. **Use it to debug, but understand the bug.** Let the agent help you find the error, then make sure you can explain
   why it happened and why the fix works. Follow the procedure in [DEBUGGING.md](DEBUGGING.md) first — `OGL_CALL`,
   the debugger and RenderDoc will often find the problem faster.
4. **Review every line it writes.** If you cannot explain a line of code in your repository, you do not own it.
   Rewrite it in your own way or ask the agent to explain it until you can.
5. **Keep the steps small.** As in [GUIDELINES.md](GUIDELINES.md): one small change, build, run, commit. Do not let
   an agent rewrite many files at once — you will not know what changed or why.
6. **Verify what it says.** Agents sound confident even when they are wrong. They often mix up OpenGL versions
   (legacy `glBegin`/`glEnd`, `glGenBuffers` + `glBindBuffer` instead of the DSA functions used in this course), invent
   functions or misremember matrix conventions. Check the documentation and run the code.

## Don't

1. **Don't ask the agent to "do assignment N".** You will learn nothing, and the next assignment builds on this one.
2. **Don't paste code you don't understand.** During the assessment you will be asked to explain and modify your code.
3. **Don't let it bypass the course rules**, e.g. remove `OGL_CALL` wrappers, use OpenGL extensions or other
   libraries than the ones provided.
4. **Don't let it touch code outside of your assignment** (`src/Application`, `src/3rdParty`, ...) unless you know
   why the change is needed.

## Agent mode

If you use an agent that edits files and runs commands on its own, you can tell it how to behave. Put instructions like
these in its instruction file (`CLAUDE.md`, `AGENTS.md`, `.github/copilot-instructions.md`, ...) or at the start of
the session:

```text
I am a student learning OpenGL. Act as a tutor: explain concepts and give hints,
do not write complete solutions to the assignments. Before changing any file,
explain what you want to change and why, and wait for my approval.
```

## Honesty

Be open about using AI. If a substantial part of your code was written or shaped by an agent, say so in the commit
message or when presenting your work. You are responsible for all the code you submit, whoever typed it.
