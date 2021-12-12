#include "Point.hpp"


Point::Point(glm::vec3 position, bool isActive): isActive(isActive)
{
    color = glm::vec3(1.0f, 1.0f, 1.0f);
    MVP = glm::mat4(1.0f);
    size = 0.05f;
    vertices = {position.x, position.y, position.z};

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);
    glEnable(GL_PROGRAM_POINT_SIZE);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void    Point::setColor(glm::vec3 new_color)
{
    color = new_color;
}

void    Point::setMVP(glm::mat4 new_MVP)
{
    MVP = new_MVP;
}

void    Point::render(Shader *shader)
{
    if (!isActive)
        return ;
    shader->use();
    glUniformMatrix4fv(glGetUniformLocation(shader->ID, "MVP"), 1, GL_FALSE, glm::value_ptr(MVP));
    glUniform3fv(glGetUniformLocation(shader->ID, "color"), 1, glm::value_ptr(color));

    glBindVertexArray(VAO);
    glDrawArrays(GL_POINTS, 0, 3);

    glBindVertexArray(0);
    glUseProgram(0);

}

void    Point::setPosition(glm::vec3 new_position)
{
    vertices = {new_position.x, new_position.y, new_position.z};

    glBindVertexArray(this->VAO);
    glBindBuffer(GL_ARRAY_BUFFER, this->VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);
    //BIND VAO and VBO 0
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}



void Point::activate() { this->isActive = true; }
void Point::deactivate() { this->isActive = false; }

Point::~Point()
{
    glDeleteVertexArrays(1, &this->VAO);
    glDeleteBuffers(1, &this->VBO);
}
