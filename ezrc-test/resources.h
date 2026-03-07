// File generated with ezrc

#pragma once

#include <string_view>

namespace test 
{
    //! Retrieves the content of an embedded resource file as a string view.
    //!
    //! This function provides read-only access to files embedded into the binary
    //! by the ezrc compiler. The returned view points directly into the
    //! read-only data section and remains valid for the entire program lifetime.
    //!
    //! @param file The filename of the resource
    //!
    //! @return A non-owning view into the resource data. The view is null-terminated
    //!         so `.data()` can be used with C-style APIs that expect null-terminated strings.
    //!
    //! @throw resource_not_found If no resource with the given @p file name exists.
    std::string_view get_resource(const std::string_view file);
}
