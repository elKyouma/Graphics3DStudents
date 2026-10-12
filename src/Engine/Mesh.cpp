//
// Created by Piotr Białas on 12/11/2021.
//

#include <iostream>

#include "Mesh.h"

#include "spdlog/spdlog.h"

#include "Application/utils.h"

namespace xe {
    Mesh::Mesh(GLsizei stride, GLsizei v_buffer_size, GLenum v_buffer_hint,
               GLsizei i_buffer_size, GLenum index_type, GLenum i_buffer_hint) :
            index_type_(index_type), stride_(stride) {
        OGL_CALL(glCreateVertexArrays(1, vao_.put()));

        OGL_CALL(glCreateBuffers(1, v_buffer_.put()));
        OGL_CALL(glNamedBufferData(v_buffer_.get(), v_buffer_size, nullptr, v_buffer_hint));

        OGL_CALL(glCreateBuffers(1, i_buffer_.put()));
        OGL_CALL(glNamedBufferData(i_buffer_.get(), i_buffer_size, nullptr, i_buffer_hint));

        // All the attributes are read from the vertex buffer attached to the binding point 0.
        OGL_CALL(glVertexArrayVertexBuffer(vao_.get(), 0, v_buffer_.get(), 0, stride_));
        OGL_CALL(glVertexArrayElementBuffer(vao_.get(), i_buffer_.get()));

        switch (index_type_) {
            case GL_UNSIGNED_BYTE:
                index_size_ = 1;
                break;
            case GL_UNSIGNED_SHORT:
                index_size_ = 2;
                break;
            case GL_UNSIGNED_INT:
                index_size_ = 4;
                break;
            default:
                SPDLOG_ERROR("Unsupported index type {:#x}, use GL_UNSIGNED_BYTE, GL_UNSIGNED_SHORT or GL_UNSIGNED_INT",
                             index_type_);
                index_size_ = 0;
                break;
        }
    }


    void Mesh::load_indices(size_t offset, size_t size, void *data) {
        OGL_CALL(glNamedBufferSubData(i_buffer_.get(), offset, size, data));
    }

    void Mesh::load_vertices(size_t offset, size_t size, void *data) {
        OGL_CALL(glNamedBufferSubData(v_buffer_.get(), offset, size, data));
    }


    void Mesh::add_attribute(xe::AttributeType attr_type, GLuint size, GLenum type, GLsizei offset) const {
        auto index = static_cast<GLuint>(attr_type);
        OGL_CALL(glEnableVertexArrayAttrib(vao_.get(), index));
        OGL_CALL(glVertexArrayAttribFormat(vao_.get(), index, size, type, GL_FALSE, offset));
        OGL_CALL(glVertexArrayAttribBinding(vao_.get(), index, 0));
    }

    void Mesh::draw() const {
        OGL_CALL(glBindVertexArray(vao_.get()));
        for (auto i = 0; i < submeshes_.size(); i++) {
            submeshes_[i].material->bind();
            auto indices = reinterpret_cast<void *>(index_size_ * submeshes_[i].start);
            OGL_CALL(glDrawElements(GL_TRIANGLES, submeshes_[i].count(), index_type_, indices));
            submeshes_[i].material->unbind();
        }
        OGL_CALL(glBindVertexArray(0u));
    }


    // The buffers are mapped with the DSA functions, which do not bind them: binding the index buffer while another
    // vertex array object is bound would replace that object's index buffer.
    void *Mesh::map_vertex_buffer() {
        void *ptr;
        OGL_CALL(ptr = glMapNamedBuffer(v_buffer_.get(), GL_WRITE_ONLY));
        return ptr;
    }

    void Mesh::unmap_vertex_buffer() {
        GLboolean unmap_status;
        OGL_CALL(unmap_status = glUnmapNamedBuffer(v_buffer_.get()));
        if (!unmap_status) {
            SPDLOG_CRITICAL("Error unmapping Mesh vertex buffer");
            exit(-1);
        }
    }

    void *Mesh::map_index_buffer() {
        void *ptr;
        OGL_CALL(ptr = glMapNamedBuffer(i_buffer_.get(), GL_WRITE_ONLY));
        return ptr;
    }

    void Mesh::unmap_index_buffer() {
        GLboolean unmap_status;
        OGL_CALL(unmap_status = glUnmapNamedBuffer(i_buffer_.get()));
        if (!unmap_status) {
            SPDLOG_CRITICAL("Error unmapping Mesh index buffer");
            exit(-1);
        }
    }


}