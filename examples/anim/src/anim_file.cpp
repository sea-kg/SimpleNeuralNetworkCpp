#include "anim_file.h"
#include <iostream>
#include <fstream>

// int to hex
#include <sstream>
#include <iomanip>

template< typename T >
std::string int_to_hex( T i )
{
  std::stringstream stream;
  stream << "0x" 
         << std::setfill ('0') << std::setw(sizeof(T)*2) 
         << std::hex << i;
  return stream.str();
}

bool readAnimFile(const std::string &sFilename, AnimFile &afile) {
    AnimFile ret;

    std::ifstream rfile;
    rfile.open(sFilename.c_str());
    if (!rfile.is_open()) {
        return false;
    }
    // rfile.seekg (0, rfile.beg);
    
    rfile.read(reinterpret_cast<char*>(&(afile.file_format_identifier)), sizeof(int32_t));
    if (afile.file_format_identifier != 808545889) {
        std::cout << "Wrong file format identifier" << std::endl;
        return false;
    }
    rfile.read(reinterpret_cast<char*>(&(afile.bounding_box_minimum[0])), sizeof(float));
    rfile.read(reinterpret_cast<char*>(&(afile.bounding_box_minimum[1])), sizeof(float));
    rfile.read(reinterpret_cast<char*>(&(afile.bounding_box_minimum[2])), sizeof(float));
    
    // std::cout << "afile.bounding_box_minimum[0] " << afile.bounding_box_minimum[0] << std::endl;
    // std::cout << "afile.bounding_box_minimum[1] " << afile.bounding_box_minimum[1] << std::endl;
    // std::cout << "afile.bounding_box_minimum[2] " << afile.bounding_box_minimum[2] << std::endl;

    rfile.read(reinterpret_cast<char*>(&(afile.bounding_box_maximum[0])), sizeof(float));
    rfile.read(reinterpret_cast<char*>(&(afile.bounding_box_maximum[1])), sizeof(float));
    rfile.read(reinterpret_cast<char*>(&(afile.bounding_box_maximum[2])), sizeof(float));

    // std::cout << "afile.bounding_box_maximum[0] " << afile.bounding_box_maximum[0] << std::endl;
    // std::cout << "afile.bounding_box_maximum[1] " << afile.bounding_box_maximum[1] << std::endl;
    // std::cout << "afile.bounding_box_maximum[2] " << afile.bounding_box_maximum[2] << std::endl;
    
    rfile.read(reinterpret_cast<char*>(&(afile.bounding_sphere_center[0])), sizeof(float));
    rfile.read(reinterpret_cast<char*>(&(afile.bounding_sphere_center[1])), sizeof(float));
    rfile.read(reinterpret_cast<char*>(&(afile.bounding_sphere_center[2])), sizeof(float));

    // std::cout << "afile.bounding_sphere_center[0] " << afile.bounding_sphere_center[0] << std::endl;
    // std::cout << "afile.bounding_sphere_center[1] " << afile.bounding_sphere_center[1] << std::endl;
    // std::cout << "afile.bounding_sphere_center[2] " << afile.bounding_sphere_center[2] << std::endl;

    rfile.read(reinterpret_cast<char*>(&(afile.bounding_sphere_radius)), sizeof(float));
    
    std::cout << "bounding_sphere_radius: " << afile.bounding_sphere_radius << std::endl;

    // number_of_bones_in_skinned_animation not int!!!!
    unsigned char a;
    rfile.read(reinterpret_cast<char*>(&a), sizeof(char));
    afile.number_of_bones_in_skinned_animation = a;
    
    // rfile.read(reinterpret_cast<char*>(&(afile.number_of_bones_in_skinned_animation)), sizeof(int32_t));
    std::cout << "number_of_bones_in_skinned_animation: " << afile.number_of_bones_in_skinned_animation << std::endl;

    afile.bones = new AnimFileBone *[afile.number_of_bones_in_skinned_animation];

    for (int i = 0; i < afile.number_of_bones_in_skinned_animation; ++i) {
        afile.bones[i] = new AnimFileBone();
        // std::cout << "i = " << i << std::endl;
        // int name_len;
        unsigned char name_len;
        rfile.read(reinterpret_cast<char*>(&name_len), sizeof(char));
        // wrong format
        afile.bones[i]->name_of_bone_length = name_len;
        if (afile.bones[i]->name_of_bone_length > 0) {
            std::cout << "Name Length: " << afile.bones[i]->name_of_bone_length << std::endl;
            afile.bones[i]->name_of_bone = new char[afile.bones[i]->name_of_bone_length];
            rfile.read(
                afile.bones[i]->name_of_bone,
                afile.bones[i]->name_of_bone_length
            );

            std::cout << "Name: " << afile.bones[i]->name_of_bone << std::endl;
        } else {
            afile.bones[i]->name_of_bone = nullptr;
        }
        rfile.read(reinterpret_cast<char*>(&(afile.bones[i]->parent_of_the_bone)), sizeof(short));
        std::cout << "Parent_of_the_bone: " << afile.bones[i]->parent_of_the_bone << std::endl;
        // std::cout << "afile.bones[i]->parent_of_the_bone " << afile.bones[i]->parent_of_the_bone << std::endl;
    }

    // wrong expected int but got char
    rfile.read(reinterpret_cast<char*>(&a), sizeof(char));
    afile.number_of_animations = a;
    std::cout << "afile.number_of_animations = " << afile.number_of_animations << std::endl;
    afile.animations = new AnimFileAnimation *[afile.number_of_animations];
    for (int i = 0; i < afile.number_of_animations; ++i) {
        afile.animations[i] = new AnimFileAnimation();

        // wrong datatime
        unsigned char name_len;
        rfile.read(reinterpret_cast<char*>(&name_len), sizeof(char));
        afile.animations[i]->name_of_animation_length = name_len;

        if (afile.animations[i]->name_of_animation_length > 0) {
            // std::cout << "Name Length: " << afile.animations[i]->name_of_animation_length << std::endl;
            afile.animations[i]->name_of_animation = new char[afile.animations[i]->name_of_animation_length];
            rfile.read(
                afile.animations[i]->name_of_animation,
                afile.animations[i]->name_of_animation_length
            );

            std::cout << "Animation name: " << afile.animations[i]->name_of_animation << std::endl;
        } else {
            afile.animations[i]->name_of_animation = nullptr;
        }
        // animations
    }

    // again
    rfile.read(reinterpret_cast<char*>(&(afile.file_format_identifier_again)), sizeof(int32_t));
    if (afile.file_format_identifier_again != 808545889) {
        std::cout << "Wrong file format identifier again" << std::endl;
        return false;
    }
   
    for (int i = 0; i < afile.number_of_bones_in_skinned_animation; ++i) {
        
        rfile.read(reinterpret_cast<char*>(&(afile.bones[i]->bone_position_along[0])), sizeof(float));
        rfile.read(reinterpret_cast<char*>(&(afile.bones[i]->bone_position_along[1])), sizeof(float));
        rfile.read(reinterpret_cast<char*>(&(afile.bones[i]->bone_position_along[2])), sizeof(float));

        rfile.read(reinterpret_cast<char*>(&(afile.bones[i]->bone_rotation_quaternion[0])), sizeof(float));
        rfile.read(reinterpret_cast<char*>(&(afile.bones[i]->bone_rotation_quaternion[1])), sizeof(float));
        rfile.read(reinterpret_cast<char*>(&(afile.bones[i]->bone_rotation_quaternion[2])), sizeof(float));
        rfile.read(reinterpret_cast<char*>(&(afile.bones[i]->bone_rotation_quaternion[3])), sizeof(float));

        rfile.read(reinterpret_cast<char*>(&(afile.bones[i]->bone_scale_in_all_directions[0])), sizeof(float));
        rfile.read(reinterpret_cast<char*>(&(afile.bones[i]->bone_scale_in_all_directions[1])), sizeof(float));
        rfile.read(reinterpret_cast<char*>(&(afile.bones[i]->bone_scale_in_all_directions[2])), sizeof(float));
    }

    for (int i = 0; i < afile.number_of_animations; ++i) {
        
        // wrong expected int but got char
        rfile.read(reinterpret_cast<char*>(&a), sizeof(char));
        afile.animations[i]->number_of_animation_bones = a;

        
        // rfile.read(reinterpret_cast<char*>(&(afile.animations[i]->number_of_animation_bones)), sizeof(int));
        std::cout << "afile.animations[i]->number_of_animation_bones " << afile.animations[i]->number_of_animation_bones << std::endl;

        rfile.read(reinterpret_cast<char*>(&(afile.animations[i]->translation_bounding_box_minimum_along[0])), sizeof(float));
        rfile.read(reinterpret_cast<char*>(&(afile.animations[i]->translation_bounding_box_minimum_along[1])), sizeof(float));
        rfile.read(reinterpret_cast<char*>(&(afile.animations[i]->translation_bounding_box_minimum_along[2])), sizeof(float));

        rfile.read(reinterpret_cast<char*>(&(afile.animations[i]->translation_bounding_box_size[0])), sizeof(float));
        rfile.read(reinterpret_cast<char*>(&(afile.animations[i]->translation_bounding_box_size[1])), sizeof(float));
        rfile.read(reinterpret_cast<char*>(&(afile.animations[i]->translation_bounding_box_size[2])), sizeof(float));

        rfile.read(reinterpret_cast<char*>(&(afile.animations[i]->scaling_bounding_box_minimum_along[0])), sizeof(float));
        rfile.read(reinterpret_cast<char*>(&(afile.animations[i]->scaling_bounding_box_minimum_along[1])), sizeof(float));
        rfile.read(reinterpret_cast<char*>(&(afile.animations[i]->scaling_bounding_box_minimum_along[2])), sizeof(float));

        rfile.read(reinterpret_cast<char*>(&(afile.animations[i]->scaling_bounding_box_size[0])), sizeof(float));
        rfile.read(reinterpret_cast<char*>(&(afile.animations[i]->scaling_bounding_box_size[1])), sizeof(float));
        rfile.read(reinterpret_cast<char*>(&(afile.animations[i]->scaling_bounding_box_size[2])), sizeof(float));

        // Flag (unsigned char), each 8 bits of which determine if the animations should store translation, scale or rotation components for bones taking part in the animations.
        rfile.read(reinterpret_cast<char*>(&(afile.animations[i]->flag)), sizeof(unsigned char));

        rfile.read(reinterpret_cast<char*>(&a), sizeof(char));
        afile.animations[i]->number_of_animation_frames = a;
        // rfile.read(reinterpret_cast<char*>(&(afile.animations[i]->number_of_animation_frames)), sizeof(int8_t));

        std::cout << "afile.animations[i]->number_of_animation_frames: " << afile.animations[i]->number_of_animation_frames << std::endl;
        

    }



    


    rfile.close();
    return true;
}
