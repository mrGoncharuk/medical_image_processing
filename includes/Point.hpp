#include "Graphics.hpp"
#include "Shader.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <vector>

class Point
{
private:
    unsigned int VBO, VAO;
    std::vector<float> vertices;
    glm::vec3 color;
    glm::mat4 MVP;
    float size;
    bool isActive;

public:
    Point(glm::vec3 position, bool isActive = true);
    ~Point();
    void setColor(glm::vec3 new_color);
    void setMVP(glm::mat4 MVP);
    void setPosition(glm::vec3 new_position);
    void render(Shader *shader);
    void activate();
    void deactivate();
};
