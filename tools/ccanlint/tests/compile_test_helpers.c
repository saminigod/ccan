#include <tools/ccanlint/ccanlint.h>
#include <tools/tools.h>
#include <ccan/talloc/talloc.h>
#include <ccan/str/str.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <limits.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <err.h>
#include <string.h>
#include <ctype.h>

static const char *can_build(struct manifest *m)
{
	if (safe_mode)
		return "Safe mode enabled";
	return NULL;
}

static char *compile(struct manifest *m,
		     bool keep,
		     struct ccan_file *cfile)
{
	cfile->compiled = maybe_temp_file(m, ".o", keep, cfile->fullname);
	return compile_object(m, cfile->fullname, ccan_dir, "",
			      cfile->compiled);
}

static void *do_compile_test_helpers(struct manifest *m,
				     bool keep,
				     unsigned int *timeleft)
{
	char *cmdout = NULL;
	struct ccan_file *i;

	compile_tests.total_score = 0;
	list_for_each(&m->other_test_c_files, i, list) {
		compile_tests.total_score++;
		cmdout = compile(m, keep, i);
		if (cmdout)
			return talloc_asprintf(m,
					       "Failed to compile helper C"
					       " code file %s:\n%s",
					       i->name, cmdout);
	}
	return NULL;
}

static const char *describe_compile_test_helpers(struct manifest *m,
						 void *check_result)
{
	return check_result;
}

struct ccanlint compile_test_helpers = {
	.key = "compile-helpers",
	.name = "Module test helper objects compile",
	.total_score = 1,
	.check = do_compile_test_helpers,
	.describe = describe_compile_test_helpers,
	.can_run = can_build,
};

REGISTER_TEST(compile_test_helpers, &depends_built, &has_tests, NULL);