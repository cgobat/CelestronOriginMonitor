/* Force-included header: pre-include stellarsolver_export.h with the right macro,
   so the original header's #pragma once prevents re-processing. */
#ifdef _WIN32
#  define stellarsolver_EXPORTS   /* trick: makes the header use dllexport */
#  include <stellarsolver_export.h>
#  undef STELLARSOLVER_API
#  define STELLARSOLVER_API       /* empty: static library, no decoration */
#  undef stellarsolver_EXPORTS
#endif
