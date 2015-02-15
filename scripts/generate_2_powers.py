file = open("../power_2_constants.h", "w")
file.write("//AUTO-GENERATED - DOO NOT EDIT - SEE generate_2_powers.py\n")
file.write("//A list of powers of 2, 1-64\n")
file.write("//Consts of the form 'two_power_*_less 64' means 2 to the power of (*-64)\n")
for power in range(0,63):
    file.write("\n");
    file.write("static const uint64_t "
          + "two_power_" + str(power) + " = "
          + str(2**power)
          + ";\n");
    file.write("static const uint64_t "
          + "two_power_" + str(power + 64) + "_less_64" + " = "
          + str(2**power)
          + ";\n");
file.close();