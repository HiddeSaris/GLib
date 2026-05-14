#include "BufferLayout.h"

#include "glad/glad.h"

namespace GLib {

    BufferElement::BufferElement(const std::string& name, GLuint base_type, uint32_t amount, bool is_matrix, bool normalized)
        : Name(name), BaseType(base_type), Amount(amount), Offset(0), IsMatrix(is_matrix), Normalized(normalized)
    {
        Size = (BaseType == GL_BOOL ? 1 : 4) * (IsMatrix ? Amount * Amount : Amount);
        
        if (!(BaseType == GL_FLOAT || BaseType == GL_INT || BaseType == GL_BOOL) ||
            Amount > 4 || Amount < 1){
            std::cout << "Wrong Arguments for BufferElement:" << "\n"
                << "Basetype: " << (BaseType == GL_FLOAT ? "GL_FLOAT" : (BaseType == GL_INT ? "GL_INT" : "GL_BOOL")) << "\n"
                << "Amount: " << Amount;
            exit(1);
        }
    }

}