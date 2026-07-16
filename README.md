# edu examples

Build the compiler first: `make` (from the project root). Then:

```
./educ examples/hello.edu && ./hello
```

Add `-i` to any invocation to keep the generated C next to the source
(`./educ -i examples/hello.edu` -> `examples/hello.c`... actually it lands
in the current directory as `hello.c`).

## Keyword glossary

| Keyword     | Meaning (French)     | English      |
|-------------|-----------------------|--------------|
| `kesa`      | Fonction               | function     |
| `de`        | Si                     | if           |
| `nensole`   | Sinon                  | else         |
| `tsiamenke` | Tant que               | while        |
| `bisou`     | Pour                   | for          |
| `delorene` / `resoucte` | Retourne  | return       |
| `ripare`    | Déclarer               | declare (variable) |
| `detsinfe`  | Variable               | declare (variable, synonym of `ripare`) |
| `ribate`    | Afficher                | print (strings -> stderr, numbers -> stdout) |
| `riba`      | Lire                    | read one line from stdin (auto-detects number vs. text) |
| `ricane`    | Écrire                  | write to stdout |

Other syntax is unchanged: `{ }` blocks, `( )` calls/grouping, `,` argument
separator, `;` statement separator, `+ - * /` arithmetic,
`< > <= >= == !=` comparisons, `-x` unary negation, `"..."` strings,
`// line comments`, `/* block comments */`.

There is no modulo operator - see `fizzbuzz.edu` and `primes.edu` for the
usual repeated-subtraction workaround. Identifiers may contain letters,
digits, and underscores, but not start with a digit.

## The examples

| File | Demonstrates |
|---|---|
| `hello.edu` | Smallest possible program. |
| `negative_numbers.edu` | Unary minus, comparisons. |
| `fib.edu` | Recursion, `while`. |
| `gcd.edu` | Recursion (Euclid's algorithm), helper functions. |
| `fizzbuzz.edu` | `else if` chains, a hand-rolled `mod` function. |
| `primes.edu` | Nested loops, early return from a function. |
| `multiplication_table.edu` | Nested `for` loops. |
| `control.edu` | `if`/`else`, `for`, functions, mixed strings/numbers. |
| `scope.edu` | Flat variable scoping: a `ripare`/`detsinfe` inside a block stays visible after the block ends. |
| `average.edu` | `riba` in a loop, computing a running sum. |
| `temperature.edu` | Single `riba`/`ricane` round trip, operator precedence (`c * 9 / 5 + 32`). |
| `io.edu` | Reading a string and a number in sequence. |
| `string_concat.edu` | `+` on strings, and mixed string/number concatenation. |
| `nested_functions.edu` | A `kesa` defined inside another function, and inside an `if` block. |

`average.edu`, `temperature.edu`, and `io.edu` read from stdin, e.g.:

```
printf "3\n10\n20\n30\n" | ./average
printf "100\n" | ./temperature
printf "Ada\n7\n" | ./io
```
