function(dev1_collect_resources BASE_DIR TARGET_LIBRARY_NAME)
    file(GLOB_RECURSE files 
        LIST_DIRECTORIES false
        CONFIGURE_DEPENDS
        "${BASE_DIR}/*.*"
    )

    set(filename_groups)
    foreach(file ${files})
        get_filename_component(filename ${file} NAME_WE)
        get_filename_component(directory ${file} DIRECTORY)
        get_filename_component(extension ${file} EXT)
        string(SUBSTRING "${extension}" 1 -1 extension)
        
        string(REPLACE "\\" "/" normalized_path "${file}")
        string(REPLACE "\"" "\\\"" escaped_path "${normalized_path}")
        
        list(FIND filename_groups "${filename}" idx)
        if(idx EQUAL -1)
            list(APPEND filename_groups "${filename}")
            list(APPEND filename_groups "${escaped_path}")
            list(APPEND filename_groups "${extension}")
            list(APPEND filename_groups "1") 
        else()
            math(EXPR count_idx "${idx} + 3")
            list(LENGTH filename_groups list_length)
            if(${count_idx} LESS ${list_length} OR ${count_idx} EQUAL ${list_length})
                list(GET filename_groups ${count_idx} count)
                math(EXPR new_count "${count} + 1")
                list(REMOVE_AT filename_groups ${count_idx})
                list(INSERT filename_groups ${count_idx} "${new_count}")
            endif()
            
            list(APPEND filename_groups "${filename}")
            list(APPEND filename_groups "${escaped_path}")
            list(APPEND filename_groups "${extension}")
            list(APPEND filename_groups "${new_count}")
        endif()
    endforeach()
    
    # Second pass: Generate the output pairs
    set(pairs)
    set(current_name "")
    set(current_entries)
    
    list(LENGTH filename_groups total_length)
    math(EXPR max_idx "${total_length} - 1")
    
    set(idx 0)
    while(${idx} LESS ${total_length})
        list(GET filename_groups ${idx} name)
        math(EXPR path_idx "${idx} + 1")
        math(EXPR ext_idx "${idx} + 2")
        math(EXPR count_idx "${idx} + 3")
        
        if(${path_idx} LESS ${total_length})
            list(GET filename_groups ${path_idx} path)
        endif()
        if(${ext_idx} LESS ${total_length})
            list(GET filename_groups ${ext_idx} ext)
        endif()
        if(${count_idx} LESS ${total_length})
            list(GET filename_groups ${count_idx} count)
        endif()
        
        if(NOT "${name}" STREQUAL "${current_name}")
            # Start new group
            if(NOT "${current_name}" STREQUAL "")
                # Generate entry for previous group
                list(JOIN current_entries "," entries_str)
                list(APPEND pairs "hana::make_pair(
                    path<${current_name_chars}>{},
                    hana::make_tuple(${entries_str}))")
            endif()
            
            set(current_name "${name}")
            set(current_entries)
            
            # Generate character array for new name
            set(current_name_chars "")
            string(LENGTH "${name}" name_len)
            foreach(char_idx RANGE ${name_len})
                if(${char_idx} LESS ${name_len})
                    string(SUBSTRING "${name}" ${char_idx} 1 char)
                    if(current_name_chars STREQUAL "")
                        set(current_name_chars "'${char}'")
                    else()
                        set(current_name_chars "${current_name_chars}, '${char}'")
                    endif()
                endif()
            endforeach()
        endif()
        
        # Get directory part
        get_filename_component(directory "${path}" DIRECTORY)
        string(REPLACE "${BASE_DIR}" "" relative_dir "${directory}")
        string(REGEX REPLACE "^/" "" relative_dir "${relative_dir}")
        
        if(relative_dir MATCHES ".*/([^/]+)$")
            set(last_dir "${CMAKE_MATCH_1}")
        else()
            set(last_dir "${relative_dir}")
        endif()
        
        # Generate directory character array
        set(dir_chars "")
        if(NOT "${last_dir}" STREQUAL "")
            string(LENGTH "${last_dir}" dir_len)
            foreach(char_idx RANGE ${dir_len})
                if(${char_idx} LESS ${dir_len})
                    string(SUBSTRING "${last_dir}" ${char_idx} 1 char)
                    if(dir_chars STREQUAL "")
                        set(dir_chars "'${char}'")
                    else()
                        set(dir_chars "${dir_chars}, '${char}'")
                    endif()
                endif()
            endforeach()
        endif()
        
        # Generate extension character array
        set(ext_chars "")
        string(LENGTH "${ext}" ext_len)
        foreach(char_idx RANGE ${ext_len})
            if(${char_idx} LESS ${ext_len})
                string(SUBSTRING "${ext}" ${char_idx} 1 char)
                if(ext_chars STREQUAL "")
                    set(ext_chars "'${char}'")
                else()
                    set(ext_chars "${ext_chars}, '${char}'")
                endif()
            endif()
        endforeach()
        
        # Create tuple entry
        list(APPEND current_entries "hana::make_tuple(path<${dir_chars}>{}, path<${ext_chars}>{}, split_at_comma(hana::tuple<${path}>{}))")
        
        math(EXPR idx "${idx} + 4")
    endwhile()
    
    # Handle last group
    if(NOT "${current_name}" STREQUAL "")
        list(JOIN current_entries "," entries_str)
        list(APPEND pairs "hana::make_pair(
            path<${current_name_chars}>{},
            hana::make_tuple(${entries_str}))")
    endif()
    
    # Join all pairs and set the compile definition
    list(JOIN pairs "," map_pairs)
    message("${map_pairs}")
    target_compile_definitions(${TARGET_LIBRARY_NAME} PRIVATE 
        "ROOT_RESOURCE_DIR=${BASE_DIR}"
        "RESOURCE_MAP_PAIRS=${map_pairs}"
    )
endfunction()