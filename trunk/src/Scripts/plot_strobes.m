function plot_strobes(nap, strobes, frame_no)
  n = squeeze(nap(frame_no,:,:));
  s = strobes(frame_no,:);
  figure;
  mesh(n, 'MeshStyle', 'row', 'EdgeColor','black');
  view([0.01 80]);
  hold on;
  for ch = 1:size(n, 1);
    st = s{1,ch};
    for i=1:length(st)
      if (st(i) > 0)
        plot3(st(i),ch,n(ch, st(i)) ,'Marker','o','MarkerFaceColor',[1 0 0],'MarkerEdgeColor','w','MarkerSize',6);
      end
    end
  end
  