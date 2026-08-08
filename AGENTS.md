# Jetski Rules

## Command Execution Policy
- **Read-Only / Non-Writing Commands**: Run all read-only and non-destructive commands (e.g., `ls`, `cat`, `grep`, `find`, `which`, `git status`, `git diff`, `git log`, `head`, `tail`, `echo`, inspecting system state and tools) automatically without prompting for confirmation.
- **Workflow & Build Commands**: Commands like `gclient`, `gn`, `ninja`, `git fetch` can be run proactively as required by the user's task.
- **Destructive Commands**: Prompt only before destructive operations (such as `rm`, `git reset --hard`, deleting branches/data).
