#include "student_code.h"
#include <common.h>

int main() {
	PROCESS* p0 = create_process(1.0f, 0.0f, 0.0f);
	PROCESS* p1 = create_process(1.0f, 1.0f, 0.0f);
	PROCESS_LIST* pl = create_process_list();
	add_process_to_tail(pl, p0);
	add_process_to_tail(pl, p1);

	PROCESS* selected = priority_process_selector(pl);
  return 0;
}
