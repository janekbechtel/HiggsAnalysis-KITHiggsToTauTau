#!/usr/bin/env python
# -*- coding: utf-8 -*-
import logging
import Artus.Utility.logger as logger
log = logging.getLogger(__name__)

import argparse
import copy
import sys
import os
import re

import Artus.Utility.jsonTools as jsonTools
import sys
import HiggsAnalysis.KITHiggsToTauTau.plotting.configs.samples_run2 as samples
import HiggsAnalysis.KITHiggsToTauTau.plotting.configs.binnings as binnings
import HiggsAnalysis.KITHiggsToTauTau.plotting.configs.labels as labels
import ROOT
import glob
import itertools
import ROOT
import matplotlib.pyplot as plt
from matplotlib import cm
ROOT.PyConfig.IgnoreCommandLineOptions = True

def plot_correlations(parameters, correlation_dicts, dir_path, channel, category, samples, dimension=7, calc_diff=False):
	sample = samples[0]
	ranges = [[-1.0,1.0],[-1.0,1.0]]
	if isinstance(channel, list):
		channel = channel[0]
	corr_list = [] #0 = MC; 1 = Data
	name_list = ["Diff", "MC", "Data"]
	for count, correlation_dict in enumerate(correlation_dicts):
		if correlation_dict is None:
			continue
		corr_vars = {}
		ws = correlation_dict["weight_sum"]
		labeldict = labels.LabelsDict()
		for varxy in correlation_dict.iterkeys():
			try:
				varx, vary = map(str, varxy.split("+-+"))
			except ValueError:
				continue
			corr_vars["var_%s"%varx] = (correlation_dict["var_%s"%varx] - (correlation_dict[varx]**2.)/ws)/ws
			corr_vars["var_%s"%vary] = (correlation_dict["var_%s"%vary] - (correlation_dict[vary]**2.)/ws)/ws
			try:
				corr_vars[varxy] = (correlation_dict[varxy]-correlation_dict[varx]*correlation_dict[vary]/ws)/ws/(
					corr_vars["var_%s"%varx])**0.5/(corr_vars["var_%s"%vary])**0.5
			except ZeroDivisionError:
				log.error("ZeroDivisonError: %s" %varxy)
				corr_vars[varxy] = 0
			except ValueError:
				log.error("ValueError: %s" %varxy)
				corr_vars[varxy] = 0
		corr_list.append(corr_vars)
		corr_vars["weight_sum"] = ws
		jsonTools.JsonDict(corr_vars).save(os.path.join(dir_path, channel, category_string, sample, "FinalCorrelation%s.json"%name_list[count]), indent=4)
	if calc_diff and len(corr_list) == 2:
		add_corr = {}
		for varxy in corr_list[0].iterkeys():
			if "+-+" in varxy:
				add_corr[varxy] = corr_list[0][varxy] - corr_list[1][varxy]
		corr_list.insert(0,add_corr)
		samples.insert(0,"MC-Data")
		ranges.insert(0,[-0.3,0.3])
	elif calc_diff:
		log.error("Diff. Correlation requested but no Data input requested")
		sys.exit()
	whole = len(parameters)/dimension
	whole = max(whole, 1)
	max_iterate = min(dimension, len(parameters))

	param_lists = []
	for i in range(whole):
		param_lists.append([])
		for j in range(max_iterate):
			param_lists[i].append(parameters[max_iterate*i+j])
	param_lists.append(parameters[max_iterate*whole:])
	if param_lists[-1] == []:
		param_lists.pop(-1)
	param_lists.append(parameters)
	special_weight = []
	for iter_count, corr_vars in enumerate(corr_list):
		v_min, v_max = ranges[iter_count]
		save_weight = []
		for i in range(len(param_lists)):
			save_weight.append([])
			for j in range(i,len(param_lists)):
				save_weight[i].append([])
				if j == len(param_lists)-1 and j != i:
					continue
				x_params = param_lists[i]
				y_params = param_lists[j]
				x_vals = []
				y_vals = []
				weights = []
				for pairs in itertools.product(x_params, y_params):
					x_vals.append(x_params.index(pairs[0])+0.5)
					y_vals.append(y_params.index(pairs[1])+0.5)
					try:
						weights.append(corr_vars["+-+".join(pairs)])
					except KeyError:
						weights.append(corr_vars["+-+".join((pairs[1],pairs[0]))])
				fig = plt.figure()
				ax = fig.add_subplot(111)
				counts, xedges, yedges, cax = ax.hist2d(x_vals, y_vals, weights=weights, bins=[len(x_params), len(y_params)], range=[(0,len(x_params)),(0,len(y_params))], cmap=cm.coolwarm, vmin=v_min, vmax=v_max)
				title_string = "Correlation Matrix: %s $\\rightarrow$ %s"%(labeldict.get_nice_label(samples[iter_count]) ,labeldict.get_nice_label("channel_%s"%channel ))
				#title_string = title_string.replace("$", "")
				#title_string = "$"+title_string+"$"
				ax.set_title(title_string)
				ax.set_xticks([x+0.5 for x in range(len(x_params))])
				ax.set_yticks([x+0.5 for x in range(len(y_params))])
				ax.set_xticks([x for x in range(len(x_params))], minor = True)
				ax.set_yticks([x for x in range(len(y_params))], minor = True)
				if j == len(param_lists)-1:
					ax.set_xticklabels([labeldict.get_nice_label(channel+"_"+x).replace(" / \\mathrm{GeV}", "").replace("\\,", "\\;") for x in x_params], rotation = 45, size='medium', va='center', ha='right', rotation_mode='anchor')
					ax.set_yticklabels([labeldict.get_nice_label(channel+"_"+x).replace(" / \\mathrm{GeV}", "").replace("\\,", "\\;") for x in y_params], rotation = 45, size='medium', va='center', ha='right', rotation_mode='anchor')
					if len(corr_list) == 3 and iter_count > 0:
						for special_weight_count,triples in enumerate(zip(x_vals, y_vals, weights)):
							#print i, j, special_weight
							ax.annotate(s="{num:.2f}\n({err:.2f})".format(num=triples[2], err=special_weight[i][j-i][special_weight_count]), xy=(triples[0],triples[1]), ha = "center", va = "center", fontsize='x-small')
					else:
						for triples in zip(x_vals, y_vals, weights):
							ax.annotate(s="%1.2f"%triples[2], xy=(triples[0],triples[1]), ha = "center", va = "center", fontsize='small')
				else:
					ax.set_xticklabels([labeldict.get_nice_label(channel+"_"+x).replace(" / \\mathrm{GeV}", "").replace("\\,", "\\;") for x in x_params], rotation = 45, size='large', va='center', ha='right', rotation_mode='anchor')
					ax.set_yticklabels([labeldict.get_nice_label(channel+"_"+x).replace(" / \\mathrm{GeV}", "").replace("\\,", "\\;") for x in y_params], rotation = 45, size='large', va='center', ha='right', rotation_mode='anchor')
					for triples in zip(x_vals, y_vals, weights):
						ax.annotate(s="%1.2f"%triples[2], xy=(triples[0],triples[1]), ha = "center", va = "center", fontsize='medium')
				# Add colorbar, make sure to specify tick locations to match desired ticklabels
				cbar = fig.colorbar(cax, ticks=[v_min, 0, v_max])
				cbar.ax.set_yticklabels([str(v_min), '0', str(v_max)])  # vertically oriented colorbar
				ax.grid(True, which="minor", linewidth = 1.5)
				ax.tick_params(axis='both', which='major', pad=5)
				plt.tight_layout()
				outname = "%sCorrelationPlot"%(name_list[iter_count])
				plt.savefig(os.path.join(dir_path, channel, category_string, sample, "%s-%i-%i_corM.png"%(outname,i,j)))
				plt.savefig(os.path.join(dir_path, channel, category_string, sample, "%s-%i-%i_corM.pdf"%(outname,i,j)))
				#plt.savefig(os.path.join(dir_path, channel, category_string, sample, "%s-%i-%i_corM.png"%(outname,i,j)))
				log.info("create plot %s" %os.path.join(dir_path, channel, category_string, sample, "%s-%i-%i_corM.png"%(outname,i,j)))
				save_weight[i][j-i] = weights
		if iter_count == 0 and len(corr_list) == 3:
			special_weight = save_weight

if __name__ == "__main__":
	parser = argparse.ArgumentParser(description="Collect and Combine Correlation Information",
									 parents=[logger.loggingParser])
	parser.add_argument("-i", "--input-dir", required=True,
						help="Input directory. Use directory of output from correlation_SampleProducer.py")
	parser.add_argument("-c", "--channels", nargs="*",
						default=["tt", "mt", "et", "em", "mm", "ee"],
						help="Channels. [Default: %(default)s]")
	parser.add_argument("--mssm", default=False, action="store_true",
	                    help="Produce the plots for the MSSM. [Default: %(default)s]")
	parser.add_argument("--mva", default=False, action="store_true",
	                    help="Produce plots for the mva studies. [Default: %(default)s]")
	parser.add_argument("--categories", nargs="+", default=["inclusive"],
	                    help="Categories. [Default: %(default)s]")
	parser.add_argument("--higgs-masses", nargs="+", default=["125"],
						help="Higgs masses. [Default: %(default)s]")
	parser.add_argument("-o", "--output-dir",
							default="SameAsInput",
							help="Output dir. [Default: %(default)s]")
	parser.add_argument("-s", "--samples", nargs="+",
						default=["ggh", "qqh", "vh", "ztt", "zll", "ttj", "vv", "wj", "data"],
						choices=["ggh", "qqh", "vh", "ztt", "zll", "ttj", "vv", "wj", "data"],
						help="Samples for correlation calculation and scatter plots. [Default: %(default)s]")
	parser.add_argument("--plot-vars", nargs="+", default=["all"],
						help = "plot correlation for those variables. [Default: %(default)s]")
	parser.add_argument("--dimension", type = int, default = 5,
						help="dimension of the output matrices. [Default: %(default)s]")
	parser.add_argument("--diff", default=False, action="store_true",
	                    help="Calculate and plot corr(MC)-corr(Data). [Default: %(default)s]")
	args = parser.parse_args()
	logger.initLogger(args)

	dir_path = os.path.expandvars(args.input_dir)
	out_path = ""
	if args.output_dir == "SameAsInput":
		out_path = dir_path
	config_list = []
	for channel in args.channels:
		for category in args.categories:
			parameters_list = []
			sample_list = []
			overall_correlations = None
			data_corrs = None
			category_string = ""
			if category != None:
				if(args.mssm):
					category_string = "catHttMSSM13TeV"
				if args.mva:
					category_string = "catMVAStudies"
				else:
					category_string = "catHtt13TeV"
				category_string = (category_string + "_{channel}_{category}").format(channel=channel, category=category)
			for sample in args.samples:
				log.debug("sample: %s; channel: %s; dir: %s"%(sample, channel, dir_path))
				info_path = os.path.join(dir_path, channel, category_string, sample, "Correlations.json")
				info_path = glob.glob(info_path)
				if len(info_path) > 1:
					log.critical("More than one file per channel-category-sample matched your discriprion --- aborting")
					sys.exit()
				info_path = info_path[0]
				config_list.append(jsonTools.JsonDict(info_path))
				config = config_list[-1]
				if len(parameters_list) == 0:
					if "all" in args.plot_vars:
						parameters_list = config["parameters_list"]
					else:
						for var in args.plot_vars:
							if var in config["parameters_list"]:
								parameters_list.append(var)
							else:
								log.error("You requested to plot correlation for variable {var}, which is not present in the calculated correlation values".format(var=var))
								sys.exit()
					#log.debug("parameters: %s" %str(parameters_list))
					#plot_correlations(parameters_list, copy.copy(config["correlations"]), out_path, channel, category_string, sample, args.dimension)
				#else:
					#log.debug("parameters: %s" %str(parameters_list))
					#plot_correlations(parameters_list, copy.copy(config["correlations"]), out_path, channel, category_string, sample, args.dimension)
				if sample == "data":
					data_corrs = copy.copy(config["correlations"])
					continue
				if overall_correlations is None:
					overall_correlations = copy.copy(config["correlations"])
				else:
					for varxy in overall_correlations.iterkeys():
						overall_correlations[varxy] += config["correlations"][varxy]
				sample_list.append(sample)
			if not os.path.exists(os.path.join(out_path, "combination", channel, category_string, "_".join(sample_list))):
				os.makedirs(os.path.join(out_path, "combination", channel, category_string, "_".join(sample_list)))
			plot_correlations(parameters_list, [overall_correlations, data_corrs], os.path.join(out_path, "combination"), channel, category_string, ["_".join(sample_list), "Data"], args.dimension, args.diff)