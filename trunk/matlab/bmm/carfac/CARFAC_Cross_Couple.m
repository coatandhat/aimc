function ears = CARFAC_Cross_Couple(ears);

n_ears = length(ears);
if n_ears > 1
  n_stages = ears(1).AGC_coeffs.n_AGC_stages;
  % now cross-ear mix the stages that updated (leading stages at phase 0):
  for stage = 1:n_stages
    if ears(1).AGC_state.decim_phase(stage) > 0
      break  % all recently updated stages are finished
    else
      mix_coeff = ears(1).AGC_coeffs.AGC_mix_coeffs(stage);
      if mix_coeff > 0  % Typically stage 1 has 0 so no work on that one.
        this_stage_sum = 0;
        % sum up over the ears and get their mean:
        for ear = 1:n_ears
          stage_state = ears(ear).AGC_state.AGC_memory(:, stage);
          this_stage_sum = this_stage_sum + stage_state;
        end
        this_stage_mean = this_stage_sum / n_ears;
        % now move them all toward the mean:
        for ear = 1:n_ears
          stage_state = ears(ear).AGC_state.AGC_memory(:, stage);
          ears(ear).AGC_state.AGC_memory(:, stage) = ...
            stage_state +  mix_coeff * (this_stage_mean - stage_state);
        end
      end
    end
  end
end
