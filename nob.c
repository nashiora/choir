#define _CRT_SECURE_NO_WARNINGS
#define _CRT_NONSTDC_NO_DEPRECATE 1

#define nob_cc(cmd) nob_cmd_append_cc(cmd)

#if defined(_MSC_VER) && !defined(__clang__)
#    define nob_cc_flags(...) // TODO: Add MSVC flags.
#else
#    define nob_cc_flags(cmd) nob_cmd_append((cmd), "-std=c23", "-Wall", "-Wextra", \
         "-Wno-unused", "-Wno-gnu-zero-variadic-macro-arguments", "-Wno-trigraphs", \
         "-Wno-unused-parameter", "-Wno-unused-function", "-Wno-unused-variable",   \
         "-Werror", "-ggdb", "-Werror=return-type", "-pedantic", "-pedantic-errors", "-fsanitize=address", \
         "-D_CRT_SECURE_NO_WARNINGS")
#endif

#if defined(_MSC_VER)
#    if defined(__clang__)
#        define nob_ar(cmd) nob_cmd_append((cmd), "llvm-lib.exe")
#    else
#        define nob_ar(cmd) nob_cmd_append((cmd), "lib.exe")
#    endif
#else
#    define nob_ar(cmd) nob_cmd_append((cmd), "ar", "rcs")
#endif

#define nob_ar_flags(cmd)

#if defined(_MSC_VER)
#    define nob_ar_output(cmd, output_path) nob_cmd_append((cmd), nob_temp_sprintf("/out:%s", (output_path)))
#else
#    define nob_ar_output(cmd, output_path) nob_cmd_append((cmd), (output_path))
#endif

#define nob_ar_inputs(cmd, ...) nob_cmd_append((cmd), __VA_ARGS__)

#if defined(_MSC_VER)
#    if defined(__clang__)
#        define nob_ld(cmd) nob_cmd_append((cmd), "clang")
#    else
#        define nob_ld(cmd) nob_cmd_append((cmd), "link.exe")
#    endif
#else
#    define nob_ld(cmd) nob_cmd_append((cmd), "ld")
#endif

#if defined(_MSC_VER)
#    if defined(__clang__)
#        define nob_ld_flags(cmd) nob_cmd_append((cmd), "-fsanitize=address")
#    else
#        define nob_ld_flags(cmd) nob_cmd_append((cmd), "/nologo")
#    endif
#else
#    define nob_ld_flags(cmd) nob_cmd_append((cmd), "-fsanitize=address")
#endif

#if defined(_MSC_VER)
#    if defined(__clang__)
#        define nob_ld_output(cmd, output_path) nob_cmd_append((cmd), "-o", (output_path))
#    else
#        define nob_ld_output(cmd, output_path) nob_cmd_append((cmd), nob_temp_sprintf("/out:%s", (output_path)))
#    endif
#else
#    define nob_ld_output(cmd, output_path) nob_cmd_append((cmd), "-o", (output_path))
#endif

#define nob_ld_inputs(cmd, ...) nob_cmd_append((cmd), __VA_ARGS__)

#if defined(_WIN32)
#    define nob_exe(exe_path) nob_temp_sprintf("%s.exe", (exe_path))
#else
#    define nob_exe(exe_path) exe_path
#endif

#if defined(_WIN32)
#    define nob_lib_a(lib_path) nob_temp_sprintf("%s.lib", (lib_path))
#else
#    define nob_lib_a(lib_path) nob_temp_sprintf("%s.a", (lib_path))
#endif

#if defined(_WIN32)
#    define nob_lib_so(lib_path) nob_temp_sprintf("%s.dll", (lib_path))
#else
#    define nob_lib_so(lib_path) nob_temp_sprintf("%s.so", (lib_path))
#endif

#define NOB_IMPLEMENTATION
#include "nob.h"

void nob_cmd_append_cc(Nob_Cmd* cmd);

#define nob_try(Result, Expr) do { if (!(Expr)) nob_return_defer(Result); } while (0)
#define nob_try_int(Result, Expr) do { if (0 != (Expr)) nob_return_defer(Result); } while (0)

void nob_cmd_append_cc(Nob_Cmd* cmd) {
    char* cc = getenv("NOB_CC");
    if (cc != NULL) {
        nob_cmd_append(cmd, cc);
        return;
    }

#if _WIN32
#  if defined(__GNUC__)
    nob_cmd_append(cmd, "cc");
#  elif defined(__clang__)
    nob_cmd_append(cmd, "clang");
#  elif defined(_MSC_VER)
    nob_cmd_append(cmd, "cl.exe");
#  endif
#else
    nob_cmd_append(cmd, "cc");
#endif
}

static Nob_String_View nob_sv_file_name(Nob_String_View path) {
    size_t begin = path.count - 1;
    while (begin > 0) {
        if (path.data[begin] == '/' || path.data[begin] == '\\') {
            begin++;
            break;
        }
        begin--;
    }

    size_t end = begin + 1;
    while (end < path.count) {
        if (path.data[end] == '.')
            break;
        end++;
    }

    path.data += begin;
    path.count = end - begin;
    return path;
}

static bool nob_read_entire_dir_recursive(const char* dir, Nob_File_Paths* paths) {
    bool result = true;

    Nob_File_Paths this_paths = {0};
    if (!nob_read_entire_dir(dir, &this_paths)) {
        nob_return_defer(false);
    }

    for (size_t i = 0; i < this_paths.count; i++) {
        const char* child_name = this_paths.items[i];
        if (child_name[0] == '.') continue;

        const char* child_path = nob_temp_sprintf("%s/%s", dir, child_name);

        Nob_File_Type child_file_type = nob_get_file_type(child_path);
        if (child_file_type == NOB_FILE_DIRECTORY) {
            nob_read_entire_dir_recursive(child_path, paths);
        } else {
            nob_da_append(paths, child_path);
        }
    }

defer:;
    nob_da_free(this_paths);
    return result;
}

static bool build_obj(const char* src, const char* obj, Nob_File_Paths* include_files, Nob_File_Paths* out_obj_files) {
    bool result = true;

    Nob_Cmd cmd = {0};

    nob_da_append(out_obj_files, obj);
    if (nob_file_exists(obj)) {
        int rebuild_status = nob_needs_rebuild1(obj, src);
        if (rebuild_status < 0) nob_return_defer(false);

        if (0 == rebuild_status) {
            rebuild_status = nob_needs_rebuild(obj, include_files->items, include_files->count);
            if (rebuild_status < 0) nob_return_defer(false);
        }

        if (0 == rebuild_status) nob_return_defer(true);
    }

    nob_cc(&cmd);
    nob_cc_output(&cmd, obj);
    nob_cmd_append(&cmd, "-c");
    nob_cc_inputs(&cmd, src);
    nob_cmd_append(&cmd, "-Iinclude");
    nob_cc_flags(&cmd);

    if (!nob_cmd_run_sync(cmd)) {
        nob_return_defer(false);
    }

defer:;
    nob_cmd_free(cmd);
    return result;
}

static bool package_lib(const char* lib, Nob_File_Paths* obj_files) {
    bool result = true;

    Nob_Cmd cmd = {0};

    int rebuild_status = nob_needs_rebuild(lib, obj_files->items, obj_files->count);
    if (rebuild_status < 0) nob_return_defer(false);
    if (0 == rebuild_status) nob_return_defer(true);

    nob_ar(&cmd);
    nob_ar_output(&cmd, lib);
    nob_ar_flags(&cmd);
    nob_da_append_many(&cmd, obj_files->items, obj_files->count);

    if (!nob_cmd_run_sync(cmd)) {
        nob_return_defer(false);
    }

defer:;
    nob_cmd_free(cmd);
    return result;
}

static bool link_exe(const char* exe, Nob_File_Paths* obj_files, Nob_File_Paths* lib_files) {
    bool result = true;

    Nob_Cmd cmd = {0};

    if (nob_file_exists(exe)) {
        int rebuild_status = nob_needs_rebuild(exe, obj_files->items, obj_files->count);
        if (rebuild_status < 0) nob_return_defer(false);

        if (0 == rebuild_status) {
            rebuild_status = nob_needs_rebuild(exe, lib_files->items, lib_files->count);
            if (rebuild_status < 0) nob_return_defer(false);
        }

        if (0 == rebuild_status) nob_return_defer(true);
    }

    nob_ld(&cmd);
    nob_ld_output(&cmd, exe);
    nob_da_append_many(&cmd, obj_files->items, obj_files->count);
    nob_ld_flags(&cmd);

    if (!nob_cmd_run_sync(cmd)) {
        nob_return_defer(false);
    }

defer:;
    nob_cmd_free(cmd);
    return result;
}

static bool build_libchoir_static(Nob_File_Paths* lib_header_files, Nob_File_Paths* lib_files) {
    bool result = true;

    Nob_File_Paths obj_files = {0};

    const char* lib_path = nob_lib_a(".build/libchoir");
    nob_da_append(lib_files, lib_path);

    for (size_t i = 0; i < lib_files->count; i++) {
        Nob_String_View file_path = nob_sv_from_cstr(lib_files->items[i]);
        if (!nob_sv_end_with(file_path, ".c")) {
            continue;
        }

        Nob_String_View file_name = nob_sv_file_name(file_path);
        if (!build_obj(lib_files->items[i], nob_temp_sprintf(".build/choir_"SV_Fmt".o", SV_Arg(file_name)), lib_header_files, &obj_files)) {
            nob_return_defer(false);
        }
    }

    if (!package_lib(lib_path, &obj_files)) {
        nob_return_defer(false);
    }

defer:;
    nob_da_free(obj_files);

    return result;
}

static bool clean(bool commit) {
    bool result = true;

    Nob_File_Paths artifacts = {0};
    nob_try(false, nob_read_entire_dir_recursive(".build", &artifacts));

    for (size_t i = 0; i < artifacts.count; i++) {
        const char* artifact_path = artifacts.items[i];
        if (commit) {
            nob_log(NOB_INFO, "Removing '%s'.", artifact_path);
            remove(artifact_path);
        } else {
            nob_log(NOB_INFO, "Would remove '%s'.", artifact_path);
        }
    }

    if (commit) {
        nob_log(NOB_INFO, "Removing '.build'.");
        remove(".build/");
    } else {
        nob_log(NOB_INFO, "Would remove '.build'.");
    }

    nob_log(NOB_INFO, "Use 'nob clean commit' to commit to removing these files.");

defer:;
    return result;
}

int main(int argc, char** argv) {
    NOB_GO_REBUILD_URSELF(argc, argv);

    // handle clean first, separately
    if (argc >= 2 && 0 == strcmp("clean", argv[1])) {
        bool commit = argc >= 3 && 0 == strcmp("commit", argv[2]);
        return clean(commit) ? 0 : 1;
    }

    int result = 0;

    Nob_File_Paths public_header_files = {0};
    Nob_File_Paths lib_header_files = {0};
    Nob_File_Paths lib_files = {0};

    Nob_Cmd cmd = {0};

    if (!nob_mkdir_if_not_exists(".build")) {
        nob_return_defer(false);
    }

    if (!nob_read_entire_dir_recursive("include", &public_header_files)) {
        nob_return_defer(1);
    }

    if (!nob_read_entire_dir_recursive("lib", &lib_files)) {
        nob_return_defer(1);
    }

    nob_da_append_many(&lib_header_files, public_header_files.items, public_header_files.count);
    for (size_t i = 0; i < lib_files.count; i++) {
        Nob_String_View file_path = nob_sv_from_cstr(lib_files.items[i]);
        if (nob_sv_end_with(file_path, ".h")) {
            nob_da_append(&lib_header_files, lib_files.items[i]);
        }
    }

    if (!build_libchoir_static(&lib_header_files, &lib_files)) {
        nob_return_defer(1);
    }

defer:;
    nob_cmd_free(cmd);
    nob_da_free(lib_files);
    nob_da_free(lib_header_files);
    nob_da_free(public_header_files);
    return result;
}
