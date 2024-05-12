#include "Defaults.h"

Defaults::Defaults(QString chambre, double temperature, double pressure,
                   double dref, double duration_factor, double stripper_response_correction, double seance_duration)
    : chambre_(chambre),
      temperature_(temperature),
      pressure_(pressure),
      dref_(dref),
      duration_factor_(duration_factor),
      stripper_response_correction_(stripper_response_correction),
      seance_duration_(seance_duration)
{}
