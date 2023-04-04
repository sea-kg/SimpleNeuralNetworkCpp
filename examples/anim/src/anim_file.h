#ifndef __EXAMPLES_ANIM_FILE_H__
#define __EXAMPLES_ANIM_FILE_H__

#include <string>
#include <cstdint>

// https://developer.unigine.com/ru/docs/future/code/formats/file_formats?rlang=cpp#anim_ff

struct AnimFileBone {
    // header
    int name_of_bone_length;
    char *name_of_bone;
    short parent_of_the_bone;

    float bone_position_along[3];
    float bone_rotation_quaternion[4];
    float bone_scale_in_all_directions[3];
};

struct AnimFileAnimationFrameBone {
    unsigned char flag;
    unsigned short translation_of_the_bone[3];
    short rotation_quaternion[4];
    unsigned short bone_scale[3];
};

struct AnimFileAnimationFrames {
    // Flag (unsigned char), each 8 bits of which determine if the current frame should store new translation, scale or rotation components for the bone. 
    // If these components have not changed since the previous frame, new data is not stored. Instead, 
    // data from the previous frame is used, which maximizes the efficiency of memory usage.

    AnimFileAnimationFrameBone *bones;

};

struct AnimFileAnimation {
    // header
    int name_of_animation_length;
    char *name_of_animation;

    int number_of_animation_bones;
    short* indices_of_the_bones_taking_part_in_the_animation;

    float translation_bounding_box_minimum_along[3];
    float translation_bounding_box_size[3];

    float scaling_bounding_box_minimum_along[3];
    float scaling_bounding_box_size[3];

    unsigned char flag;
    // Flag (unsigned char), each 8 bits of which determine if the animations should store translation, scale or rotation components for bones taking part in the animations.

    int number_of_animation_frames;
    AnimFileAnimationFrames *animation_frames;
};

struct AnimFile {
    // int32: File format identifier (int32 "an10" ('a' | ('n' << 8) | ('1' << 16) | ('0' << 24)))
    int32_t file_format_identifier;

    float bounding_box_minimum[3];
    float bounding_box_maximum[3];
    float bounding_sphere_center[3];
    float bounding_sphere_radius;
    int32_t number_of_bones_in_skinned_animation;
    AnimFileBone** bones;
    int32_t number_of_animations;
    AnimFileAnimation* animations;
    int32_t file_format_identifier_again;

    int32_t file_format_identifier_again2;
};


bool readAnimFile(const std::string &sFilename, AnimFile &afile);

#endif // __EXAMPLES_ANIM_FILE_H__