
# Lexical Analyzer in C

This project implements a **Lexical Analyzer (Lexer)** for a C-like programming language. The lexer reads source code, identifies various tokens such as keywords, operators, identifiers, literals, and comments, and performs basic error checking (like unmatched parentheses or missing semicolons).

---

## 📂 Project Structure

```
├── lexer.c                # Core logic for tokenization
├── lexer.h                # Header file with token definitions and function prototypes
├── main.c                 # Main driver file for CLI-based execution
├── sample.c               # Sample input C program to be tokenized
├── README.md              # Project documentation (this file)
```

---

## 🔍 Features

- Token classification:
  - Keywords (`int`, `float`, `if`, `else`, ...`)
  - Identifiers (`main`, `printf`, ...`)
  - Constants (numeric and character)
  - Operators (`+`, `-`, `=`, ...`)
  - Special characters (`;`, `{`, `}`, ...`)
  - Preprocessor directives (`#include`, `#define`, ...`)
  - String and character literals
  - Comments (single-line `//` and multi-line `/* */`)
- Tracks line numbers, brackets, and syntax structure
- Provides detailed error reporting for:
  - Missing semicolons
  - Mismatched parentheses/brackets
  - Unmatched quotes

---

## 🛠️ Compilation and Execution

### 🔧 Step 1: Compile

```bash
gcc lexer.c main.c -o lexer
```

> Make sure all files (`lexer.c`, `lexer.h`, `main.c`, and `sample.c`) are in the same directory.

### ▶️ Step 2: Run

```bash
./lexer sample.c
```

> The program will analyze the `sample.c` file and output token classification along with any syntax errors.

---

## 🧾 Token Types (Defined in `lexer.h`)

| Token Type               | Description                             |
|--------------------------|-----------------------------------------|
| `KEYWORD`                | Reserved C keywords                     |
| `IDENTIFIER`             | Recognized standard function names      |
| `VARIABLE`               | User-defined identifiers                |
| `OPERATOR`               | Arithmetic, logical, or assignment ops  |
| `SPECIAL_CHARACTER`      | Symbols like `()`, `{}`, `;`, etc.      |
| `PREPROCESSOR_DIRECTIVE`| Lines starting with `#`                 |
| `CONSTANT`               | Numeric literals                        |
| `STRING_LITERAL`         | Quoted string values                    |
| `COMMENT`                | Both single-line and multi-line         |
| `UNKNOWN`                | Malformed or unrecognized token         |

---

## 🧪 Sample Input (sample.c)

```c
#include <stdio.h>
int main()
{
    float f = 57.89;
    char ch = 'A';
    int sum_n = 31 + 7;
    printf("Enter the character: ");
    scanf("%c", &ch);

    printf("Sum of %c is %d\n", ch, sum_n);
}
```

---

## 📊 Output Example

```
----------------------------------------------------------------------------------------------------------------------------------
    Type : PREPROCESSOR_DIRECTIVE     ---->     Token : #include
----------------------------------------------------------------------------------------------------------------------------------
    Type : KEYWORD                    ---->     Token : int
----------------------------------------------------------------------------------------------------------------------------------
    Type : IDENTIFIER                 ---->     Token : main
...
```

---

## 🚨 Error Handling

- If a semicolon (`;`) is missing at the end of a statement, the lexer will show an error with line number.
- It verifies matching of:
  - Parentheses `()`
  - Curly braces `{}`
  - Quotes `' '` and `" "`

---

## 📌 Notes

- This lexer supports a limited set of C language features and is designed for educational purposes.
- It can be extended to include more token types and better grammar handling.

---

## 👨‍💻 Author

**Raghu G S**  
Engineering Student | Embedded Systems Enthusiast

---

## 📝 License

This project is open-source and available under the [MIT License](LICENSE).
