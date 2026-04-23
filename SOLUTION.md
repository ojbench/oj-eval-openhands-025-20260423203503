# Solution Implementation Summary

## Problem
Implement a plagiarism detection system with two components:
1. **Cheat**: Transform code to avoid plagiarism detection
2. **Anticheat**: Detect similarity between two programs

## Implementation

### Combined Solution (`main.cpp`)
I've implemented a combined solution that detects which mode to run based on the input:
- **Cheat mode**: If only one program is provided (no "endprogram" separator)
- **Anticheat mode**: If two programs are provided (with "endprogram" separators)

### Cheat Strategy
The cheat program applies multiple transformations to make code look different:
1. **Variable renaming**: Adds hash-based prefixes to all variable names
2. **Expression wrapping**: Occasionally wraps expressions in identity operations (e.g., `x` → `(+ x 0)`)
3. **Literal transformation**: Sometimes transforms literals into equivalent expressions
4. **Dummy statements**: Adds harmless dummy assignments in code blocks

### Anticheat Strategy
The anticheat program uses multiple heuristics to calculate similarity:
1. **Total node count similarity**
2. **Structure similarity** (if/for/block counts)
3. **Statement count similarity**
4. **Variable name Jaccard similarity**
5. **Function name similarity**
6. **Control flow ratios**
7. **Call expression counts**
8. **Literal counts**
9. **Overall size difference**

The final similarity score is a weighted average of these metrics.

## Build System
- **Makefile**: Builds the `code` executable
- **CMakeLists.txt**: Supports cmake-based builds
- **Dependencies**: C++17, standard library

## Testing
The solution has been tested locally and works correctly:
- Cheat mode successfully transforms programs
- Anticheat mode calculates reasonable similarity scores

## Git Repository
- Repository: https://github.com/ojbench/oj-eval-openhands-025-20260423203503.git
- All code committed and pushed successfully
- Commit hash: a20f7ab

## Submission Status
**ISSUE**: Unable to submit to ACMOJ

### Error Details
- API Endpoint: `https://acm.sjtu.edu.cn/OnlineJudge/api/v1/problem/1939/submit`
- Error: `400 Bad Request - unable to create submission`
- Problem status: `languages_accepted: []` (empty)

### Attempts Made
1. Multiple submission attempts with different URL formats
2. Tried both HTTPS and SSH git URLs
3. Tried different `language` parameter values
4. Verified repository is accessible
5. Waited for rate limits to clear
6. Checked API authentication (token is valid)

### Conclusion
The problem appears to not be configured to accept submissions on ACMOJ. The `languages_accepted` field being empty suggests the problem is in a special configuration state or the submission system is not yet enabled for this problem.

## Files
- `main.cpp`: Combined cheat/anticheat implementation
- `lang.cpp`, `lang.h`: Language parser and evaluator (from reference)
- `transform.h`: AST transformation framework (from reference)
- `visitor.h`: AST visitor pattern (from reference)
- `Makefile`: Build configuration
- `CMakeLists.txt`: CMake configuration
- `.gitignore`: Git ignore rules

## How to Build and Run
```bash
# Build
make

# Test cheat mode
./code < test1.p

# Test anticheat mode
./code < test2.txt
```

## Next Steps
If the ACMOJ system is fixed to accept submissions, the code is ready and can be submitted immediately from the GitHub repository.
