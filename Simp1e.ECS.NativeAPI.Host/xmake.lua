target("Simp1e.ECS.NativeAPI.Host")
    set_kind("headeronly")
    add_logging()
    add_deps("Simp1e.ECS.NativeAPI")
    add_includedirs("include", {public = true})
    add_headerfiles("include/(**.h)")
