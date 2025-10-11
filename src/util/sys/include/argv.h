/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: src/util/sys/include/argv.h
 * PURPOSE: CLI argument structure and parser declaration
 * Created by: Umicom Foundation | Author: Sammy Hegab | Date: 2025-10-01 | MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_ARGV_H
#define UMICOM_ARGV_H

#include <stdbool.h>  /* For bool fields below */

/*-----------------------------------------------------------------------------
 * UmiArgv
 *
 * PURPOSE:
 *   Aggregates commonly used CLI switches/parameters so the rest of the
 *   application can consume a single struct instead of raw argv scanning.
 *   Extend this struct as features grow; keep defaults meaningful.
 *
 * FIELDS:
 *   help     - true if the user requested usage help (e.g., -h/--help).
 *   version  - true if the user requested app version.
 *   project  - optional project path/name argument.
 *   file     - optional single file path argument.
 *---------------------------------------------------------------------------*/
typedef struct {
  bool        help;
  bool        version;
  const char *project;
  const char *file;
} UmiArgv;

/*-----------------------------------------------------------------------------
 * umi_argv_parse
 *
 * PURPOSE:
 *   Parse argc/argv into a UmiArgv struct. Does not mutate the original argv.
 *
 * RETURNS:
 *   true on successful parse; false on invalid argument combination.
 *
 * NOTES:
 *   - 'out' must be a valid pointer; fields are set to defaults first.
 *   - Rich error reporting (e.g., which flag failed) can be layered later.
 *---------------------------------------------------------------------------*/
bool umi_argv_parse(int argc, char **argv, UmiArgv *out);

#endif /* UMICOM_ARGV_H */
