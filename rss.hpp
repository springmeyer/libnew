#pragma once

#include <iostream>

#ifdef __APPLE__
# include <mach/task.h>
# include <mach/mach_init.h>
#endif

// http://stackoverflow.com/a/372525
/// The amount of memory currently being used by this process, in bytes.
/// By default, returns the full virtual arena, but if resident=true,
/// it will report just the resident set in RAM (if supported on that OS).
void memory_used(std::string const& msg="")
{
#if defined(__APPLE__)
    task_basic_info_data_t t_info;
    mach_msg_type_number_t t_info_count = TASK_BASIC_INFO_COUNT;
    kern_return_t err;
    err = task_info(current_task(), TASK_BASIC_INFO, (task_info_t)&t_info, &t_info_count);
    (void) &err;
    std::clog << msg << " rss: " << t_info.resident_size << "/" << t_info.virtual_size << "\n";
#endif
}
