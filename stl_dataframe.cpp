#include <iostream>
#include <array>
#include <string>
#include <vector>
#include <list>
#include <ranges>
#include <numeric>
#include <algorithm>

// Unscoped enumeration for gender.
enum Gender {
    MALE, 
    FEMALE
};

// Templated structure holding fixed-size arrays for person data.
template <std::size_t N>
struct Person {
    std::array<std::string, N> first_name{};
    std::array<std::string, N> middle_name{};
    std::array<std::string, N> surname{};
    std::array<int, N> age{};
    std::array<Gender, N> gender{}; 
};

// Function to fill data for 10 people with simulated records.
void fill_data(Person<10>& people) {
    people.first_name[0] = "John";
    people.middle_name[0] = "A.";
    people.surname[0] = "Doe";
    people.age[0] = 30;
    people.gender[0] = MALE;
    
    people.first_name[1] = "Jane";
    people.middle_name[1] = "B.";
    people.surname[1] = "Smith";
    people.age[1] = 25;
    people.gender[1] = FEMALE;
    
    people.first_name[2] = "Michael";
    people.middle_name[2] = "C.";
    people.surname[2] = "Johnson";
    people.age[2] = 40;
    people.gender[2] = MALE;
    
    people.first_name[3] = "Emily";
    people.middle_name[3] = "D.";
    people.surname[3] = "Davis";
    people.age[3] = 35;
    people.gender[3] = FEMALE;
    
    people.first_name[4] = "William";
    people.middle_name[4] = "E.";
    people.surname[4] = "Brown";
    people.age[4] = 28;
    people.gender[4] = MALE;
    
    people.first_name[5] = "Olivia";
    people.middle_name[5] = "F.";
    people.surname[5] = "Wilson";
    people.age[5] = 22;
    people.gender[5] = FEMALE;
    
    people.first_name[6] = "James";
    people.middle_name[6] = "G.";
    people.surname[6] = "Miller";
    people.age[6] = 33;
    people.gender[6] = MALE;
    
    people.first_name[7] = "Sophia";
    people.middle_name[7] = "H.";
    people.surname[7] = "Taylor";
    people.age[7] = 27;
    people.gender[7] = FEMALE;
    
    people.first_name[8] = "Benjamin";
    people.middle_name[8] = "I.";
    people.surname[8] = "Anderson";
    people.age[8] = 45;
    people.gender[8] = MALE;
    
    people.first_name[9] = "Isabella";
    people.middle_name[9] = "J.";
    people.surname[9] = "Thomas";
    people.age[9] = 31;
    people.gender[9] = FEMALE;
}

void display_person(Person<10> & people){
    auto indices = std::views::iota(0, 10) | 
                   std::ranges::to<std::vector>();
    
    std::ranges::for_each(indices, [&people](size_t idx) {
        std::cout << "Name : " << people.first_name[idx] << " " 
                               << people.middle_name[idx] << " " 
                               << people.surname[idx] << '\n'
                  << "Gender : " << (people.gender[idx] == MALE ? "Male" : "Female") << '\n'
                  << "Age : " << people.age[idx] << "\n\n";
    });
}

void display_sorted_person(Person<10> & people){
    auto indices = std::views::iota(0, 10) | 
                   std::ranges::to<std::vector>();
    
    std::ranges::sort(indices, [&people](std::size_t idx, std::size_t jdx){
        return people.age[idx] < people.age[jdx]; 
    });
    
    std::ranges::for_each(indices, [&people](size_t idx) {
        std::cout << "Name : " << people.first_name[idx] << " " 
                               << people.middle_name[idx] << " " 
                               << people.surname[idx] << '\n'
                  << "Gender : " << (people.gender[idx] == MALE ? "Male" : "Female") << '\n'
                  << "Age : " << people.age[idx] << "\n\n";
    });
}


void display_male(Person<10> & people){
    auto indices = std::views::iota(0, 10) | 
                   std::views::filter([&people](size_t idx){
                       return people.gender[idx] == MALE;
                   }) |
                   std::ranges::to<std::vector>();

    std::ranges::for_each(indices, [&people](size_t idx) {
        std::cout << "Name : " << people.first_name[idx] << " " 
                               << people.middle_name[idx] << " " 
                               << people.surname[idx] << '\n'
                  << "Gender : " << (people.gender[idx] == MALE ? "Male" : "Female") << '\n'
                  << "Age : " << people.age[idx] << "\n\n";
    });    
}

void display_sorted_male(Person<10> & people){
    auto indices = std::views::iota(0, 10) | 
                   std::views::filter([&people](size_t idx){
                       return people.gender[idx] == MALE;
                   }) |
                   std::ranges::to<std::vector>();
    
    std::ranges::sort(indices, [&people](std::size_t idx, std::size_t jdx){
        return people.age[idx] < people.age[jdx]; 
    });

    std::ranges::for_each(indices, [&people](size_t idx) {
        std::cout << "Name : " << people.first_name[idx] << " " 
                               << people.middle_name[idx] << " " 
                               << people.surname[idx] << '\n'
                  << "Gender : " << (people.gender[idx] == MALE ? "Male" : "Female") << '\n'
                  << "Age : " << people.age[idx] << "\n\n";
    });    
}

void display_female(Person<10> & people){
    auto indices = std::views::iota(0, 10) | 
                   std::views::filter([&people](size_t idx){
                       return people.gender[idx] == FEMALE;
                   }) |
                   std::ranges::to<std::vector>();

    std::ranges::for_each(indices, [&people](size_t idx) {
        std::cout << "Name : " << people.first_name[idx] << " " 
                               << people.middle_name[idx] << " " 
                               << people.surname[idx] << '\n'
                  << "Gender : " << (people.gender[idx] == MALE ? "Male" : "Female") << '\n'
                  << "Age : " << people.age[idx] << "\n\n";
    });    
}

void display_sorted_female(Person<10> & people){
    auto indices = std::views::iota(0, 10) | 
                   std::views::filter([&people](size_t idx){
                       return people.gender[idx] == FEMALE;
                   }) |
                   std::ranges::to<std::vector>();
                   
    std::ranges::sort(indices, [&people](std::size_t idx, std::size_t jdx){
        return people.age[idx] < people.age[jdx]; 
    });

    std::ranges::for_each(indices, [&people](size_t idx) {
        std::cout << "Name : " << people.first_name[idx] << " " 
                               << people.middle_name[idx] << " " 
                               << people.surname[idx] << '\n'
                  << "Gender : " << (people.gender[idx] == MALE ? "Male" : "Female") << '\n'
                  << "Age : " << people.age[idx] << "\n\n";
    });    
}

void display_avg_age_male(Person<10> & people){
    auto indices = std::views::iota(0, 10) | 
                   std::views::filter([&people](size_t idx){
                       return people.gender[idx] == MALE;
                   }) |
                   std::ranges::to<std::vector>();
                   
    int sum_age = 0;
    std::ranges::for_each(indices, [&people, &sum_age](size_t idx){
        sum_age += people.age[idx];
    });
    
    float avg_age = static_cast<float>(sum_age) / static_cast<float>(indices.size());
    
    std::cout << "Average male age is :" << avg_age << '\n';
}

void display_avg_age_female(Person<10> & people){
    auto indices = std::views::iota(0, 10) | 
                   std::views::filter([&people](size_t idx){
                       return people.gender[idx] == FEMALE;
                   }) |
                   std::ranges::to<std::vector>();
                   
    int sum_age = 0;
    std::ranges::for_each(indices, [&people, &sum_age](size_t idx){
        sum_age += people.age[idx];
    });
    
    float avg_age = static_cast<float>(sum_age) / static_cast<float>(indices.size());
    
    std::cout << "Average male age is :" << avg_age << '\n';
}


int main(){
    Person<10> people;
    fill_data(people);
    
    display_person(people);
    std::cout << "\n------------------------------------------------------\n" ;
    display_sorted_person(people);
    std::cout << "\n------------------------------------------------------\n" ;
    display_male(people);
    std::cout << "\n------------------------------------------------------\n" ;
    display_female(people);
    std::cout << "\n------------------------------------------------------\n" ;
    display_sorted_male(people);
    std::cout << "\n------------------------------------------------------\n" ;
    display_sorted_female(people);
    std::cout << "\n------------------------------------------------------\n" ;
    display_avg_age_male(people);
    std::cout << "\n------------------------------------------------------\n" ;
    display_avg_age_female(people);
    
    return 0;
}
